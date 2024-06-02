#include "hpx_rs_defs.h"

template <> std::uint64_t fibonacci(std::uint64_t n) {
  if (n < 2)
    return n;

  std::uint64_t n1 = fibonacci(n - 1);
  std::uint64_t n2 = fibonacci(n - 2);

  return n1 + n2;
}

std::uint64_t fibonacci_hpx(std::uint64_t n) {
  if (n < 2)
    return n;

  hpx::future<std::uint64_t> n1 = hpx::async(fibonacci_hpx, n - 1);
  hpx::future<std::uint64_t> n2 = hpx::async(fibonacci_hpx, n - 2);

  return n1.get() + n2.get(); // wait for the Futures to return their values
}


void copy_parallel(const int* src, const int* src_end, int* dest) {
    hpx::copy(hpx::execution::par, src, src_end, dest);
}

std::int32_t hpx_init() {
    return hpx::init();
}

int hpx_main(int argc, char* argv[]) {
    int result = hpx_main_rust(argc, argv);

    return hpx::finalize();
}

int main(int argc, char* argv[]) {
    return hpx::init(argc, argv);
}
