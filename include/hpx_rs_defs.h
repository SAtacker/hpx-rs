#include <hpx/algorithm.hpp>
#include <hpx/chrono.hpp>
#include <hpx/execution.hpp>
#include <hpx/format.hpp>
#include <hpx/future.hpp>
#include <hpx/hpx_start.hpp>

#include <cstdint>
#include <hpx/hpx_init.hpp>
#include <hpx/iostream.hpp>
#include <iostream>

#include "rust/cxx.h"

template <typename T> T fibonacci(T n);

std::uint64_t fibonacci_hpx(std::uint64_t n);

inline std::int32_t start(rust::Fn<int(int, char **)> rust_fn, int argc,
                          char **argv) {
  //   int argc = 2;
  //   char *argv[] = {"--hpx:threads=4", "--hpx:print-bind"};

  return hpx::init(
      [&](int argc, char **argv) {
        std::cout << "launching hpx\n";
        return rust_fn(argc, argv);
      },
      argc, argv);
}

inline std::int32_t stop() { return hpx::finalize(); }

extern "C" std::int32_t hpx_init();
extern "C" int hpx_main_rust(int argc, char* argv[]);
extern "C" void copy_parallel(const int* src, const int* src_end, int* dest);
