cmake_minimum_required(VERSION 3.20)
project(bmp CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

add_library(bmp_warnings INTERFACE)
target_compile_options(bmp_warnings INTERFACE
  -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion
  $<$<CONFIG:Debug>:-g -O0 -fsanitize=address,undefined -fno-omit-frame-pointer>
  $<$<CONFIG:Release>:-O3>
)
target_link_options(bmp_warnings INTERFACE
  $<$<CONFIG:Debug>:-fsanitize=address,undefined>
)

add_library(exchange_core
  # src/core/ring_buffer.cpp
  src/core/sequence_tracker.cpp
  src/core/byte_reader.cpp
)
target_include_directories(exchange_core PUBLIC include)
target_link_libraries(exchange_core PRIVATE bmp_warnings)

add_library(itch_protocol src/protocols/itch/itch_decoder.cpp)
target_link_libraries(itch_protocol PUBLIC exchange_core)
target_link_libraries(itch_protocol PRIVATE bmp_warnings)

# add_library(ouch_protocol src/protocols/ouch/ouch_decoder.cpp src/protocols/ouch/ouch_encoder.cpp)
# target_link_libraries(ouch_protocol PUBLIC exchange_core)
# target_link_libraries(ouch_protocol PRIVATE bmp_warnings)

# add_library(exchange_transport
#   src/transport/udp_multicast.cpp
#   src/transport/tcp_session.cpp
# )
# target_link_libraries(exchange_transport PUBLIC exchange_core)
# target_link_libraries(exchange_transport PRIVATE bmp_warnings)

add_executable(bmp
  src/main.cpp
  # src/decoder_factory.cpp
  # src/config_loader.cpp
)
target_link_libraries(bmp PRIVATE
  exchange_core
  # exchange_transport
  itch_protocol
  # ouch_protocol
  bmp_warnings
)

option(BUILD_TESTS "Build unit tests" ON)
if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

option(BUILD_BENCHMARKS "Build benchmarks" OFF)
if(BUILD_BENCHMARKS)
  add_subdirectory(benchmarks)
endif()
