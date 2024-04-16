#include <hpx/algorithm.hpp>
#include <hpx/chrono.hpp>
#include <hpx/future.hpp>
#include <hpx/modules/iterator_support.hpp>

#include <memory>

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>

#include "rust/cxx.h"

// bool header = true; // print csv heading
double k = 0.5; // heat transfer coefficient
double dt = 1.; // time step
double dx = 1.; // grid spacing

inline std::size_t idx(std::size_t i, std::size_t size) {
  return (std::int64_t(i) < 0) ? (i + size) % size : i % size;
}

///////////////////////////////////////////////////////////////////////////////
// Our partition data type
struct partition_data {
  explicit partition_data(std::size_t size)
      : data_(new double[size]), size_(size) {}

  partition_data(std::size_t size, double initial_value)
      : data_(new double[size]), size_(size) {
    double base_value = double(initial_value * size);
    for (std::size_t i = 0; i != size; ++i)
      data_[i] = base_value + double(i);
  }

  double &operator[](std::size_t idx) { return data_[idx]; }
  double operator[](std::size_t idx) const { return data_[idx]; }

  std::size_t size() const { return size_; }

private:
  std::shared_ptr<double[]> data_;
  std::size_t size_;
};

inline std::ostream &operator<<(std::ostream &os, partition_data const &c) {
  os << "{";
  for (std::size_t i = 0; i != c.size(); ++i) {
    if (i != 0)
      os << ", ";
    os << c[i];
  }
  os << "}";
  return os;
}

typedef hpx::shared_future<partition_data> partition;
typedef std::vector<partition> space;
typedef hpx::future<space> amoolspace;

std::shared_ptr<space>
get_space_from_amool_space(std::shared_ptr<amoolspace> as) {
  return std::make_shared<space>(as->get());
}

///////////////////////////////////////////////////////////////////////////////
struct stepper {
  // Our data for one time step

  // Our operator
  static double heat(double left, double middle, double right) {
    return middle + (k * dt / (dx * dx)) * (left - 2 * middle + right);
  }

  // The partitioned operator, it invokes the heat operator above on all
  // elements of a partition.
  static partition_data heat_part(partition_data const &left,
                                  partition_data const &middle,
                                  partition_data const &right) {
    std::size_t size = middle.size();
    partition_data next(size);

    typedef hpx::util::counting_iterator<std::size_t> iterator;

    next[0] = heat(left[size - 1], middle[0], middle[1]);

    hpx::for_each(hpx::execution::par, iterator(1), iterator(size - 1),
                  [&next, &middle](std::size_t i) {
                    next[i] = heat(middle[i - 1], middle[i], middle[i + 1]);
                  });

    next[size - 1] = heat(middle[size - 2], middle[size - 1], right[0]);

    return next;
  }

  // do all the work on 'np' partitions, 'nx' data points each, for 'nt'
  // time steps
  std::shared_ptr<amoolspace> do_work(std::size_t np, std::size_t nx,
                                      std::size_t nt) const {
    // U[t][i] is the state of position i at time t.
    std::vector<space> U(2);
    for (space &s : U)
      s.resize(np);

    // Initial conditions: f(0, i) = i
    for (std::size_t i = 0; i != np; ++i)
      U[0][i] = hpx::make_ready_future(partition_data(nx, double(i)));

    auto Op = hpx::unwrapping(&stepper::heat_part);

    // Actual time step loop
    for (std::size_t t = 0; t != nt; ++t) {
      space const &current = U[t % 2];
      space &next = U[(t + 1) % 2];

      typedef hpx::util::counting_iterator<std::size_t> iterator;

      hpx::for_each(hpx::execution::par, iterator(0), iterator(np),
                    [&next, &current, np, &Op](std::size_t i) {
                      next[i] = hpx::dataflow(
                          hpx::launch::async, Op, current[idx(i - 1, np)],
                          current[i], current[idx(i + 1, np)]);
                      //   std::cout << "i : " << i << " current[i] " <<
                      //   current[i].get()[0]
                      //             << "\n";
                    });
    }

    return std::make_shared<amoolspace>(hpx::when_all(U[nt % 2]));
  }
};

void inline rust_wait_all_space(std::shared_ptr<space> t) { hpx::wait_all(*t); }

inline auto new_stepper() { return std::make_shared<stepper>(); }

inline void rust_print_space(std::shared_ptr<space> t) {
  auto solution = *t;
  for (std::size_t i = 0; i < solution.size(); ++i)
    std::cout << "U[" << i << "] = " << solution[i].get() << std::endl;
}

///////////////////////////////////////////////////////////////////////////////
inline void amool() {
  // Create the stepper object
  stepper step;

  // Measure execution time.
  std::uint64_t t = hpx::chrono::high_resolution_clock::now();
  std::uint64_t elapsed = 0;

  //   try {
  // Execute nt time steps on nx grid points and print the final solution.
  auto result = (step.do_work(10, 10, 45));

  space solution = result->get();

  // std::cout << "hpx threead id " << hpx::this_thread::get_id() << " "
  //           << hpx::threads::get_thread_count() << "\n";

  hpx::wait_all(solution);

  // space solution = (*result).get();

  elapsed = hpx::chrono::high_resolution_clock::now() - t;
  for (std::size_t i = 0; i < solution.size(); ++i)
    std::cout << "U[" << i << "] = " << solution[i].get() << std::endl;
  // }
  //   } catch (const std::exception &e) {
  //     std::cerr << e.what() << '\n';
  //   }

  std::cout << "\ncpp elapased: " << elapsed << "\n";
}