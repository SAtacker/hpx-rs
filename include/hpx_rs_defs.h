#include <hpx/chrono.hpp>
#include <hpx/format.hpp>
#include <hpx/future.hpp>
#include <hpx/hpx_start.hpp>

#include <cstdint>
#include <hpx/iostream.hpp>
#include <iostream>

template <typename T> T fibonacci(T n);

std::uint64_t fibonacci_hpx(std::uint64_t n);

inline std::int32_t start() { return hpx::start(nullptr, 0, nullptr); }

inline std::int32_t stop() {
  hpx::post([]() { hpx::finalize(); });
  return hpx::stop();
}