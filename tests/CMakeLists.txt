include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.18.0
)
FetchContent_MakeAvailable(googletest)

add_executable(unit_tests
  unit/test_itch_decoder.cpp
  # unit/test_ouch_decoder.cpp
)
target_link_libraries(unit_tests PRIVATE
  exchange_core
  itch_protocol
  # ouch_protocl
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(unit_tests)
