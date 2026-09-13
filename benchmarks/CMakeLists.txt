include(FetchContent)
FetchContent_Declare(
  benchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG v1.9.5
)

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABBLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(benchmark)

add_executable(bench_decode bench_decode_throughput.cpp)
target_link_libraries(bench_decode PRIVATE
  exchange_core
  itch_protocol
  # ouch_protocol
  benchmark::benchmark
)

find_package(Threads REQUIRED)
target_link_libraries(bench_decode PRIVATE Threads::Threads)
