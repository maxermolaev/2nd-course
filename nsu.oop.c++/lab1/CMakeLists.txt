cmake_minimum_required(VERSION 3.5.0)
project(BitArray VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

add_executable(BitArray source/main.cpp source/BitArray.cpp) 

add_executable(BitArray_test tests/BitArray_test.cpp source/BitArray.cpp)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)
FetchContent_MakeAvailable(googletest)

target_link_libraries(BitArray_test GTest::gtest_main)

include(GoogleTest)

gtest_discover_tests(BitArray_test)
enable_testing()