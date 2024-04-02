#include <hpx/chrono.hpp>
#include <hpx/format.hpp>
#include <hpx/future.hpp>
#include <hpx/init.hpp>

#include <cstdint>
#include <hpx/iostream.hpp>
#include <iostream>

template <typename T> T fibonacci(T n);

std::uint64_t fibonacci_hpx(std::uint64_t n);

inline std::int32_t init() {
  return hpx::init(
      [](int argc, char *argv[]) {
        hpx::cout << "fib (hpx) (10): " << fibonacci_hpx(10) << "\n";
        return hpx::finalize();
      },
      0, nullptr);
}

inline std::int32_t finalize() { return hpx::finalize(); }