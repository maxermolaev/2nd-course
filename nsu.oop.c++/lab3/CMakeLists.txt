cmake_minimum_required(VERSION 3.23)
project(SoundProcessor)

set(CMAKE_CXX_STANDARD 17)

set(exe_name ${PROJECT_NAME})
set(test_name Tests)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/58d77fa8070e8cec2dc1ed015d66b454c8d78850.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(FetchContent)
FetchContent_Declare(
        argparse
        URL https://github.com/p-ranav/argparse/archive/refs/tags/v2.9.zip
)
FetchContent_MakeAvailable(argparse)

add_library(corelib OBJECT
        src/config_parser.cpp
        src/converters_factory.cpp
        src/processor.cpp
        src/streams.cpp
        src/utils.cpp
        src/wav.cpp

        src/converters/converters_utils.cpp
        src/converters/cut_converter.cpp
        src/converters/mix_converter.cpp
        src/converters/mute_converter.cpp
        )

add_executable(${exe_name} src/main.cpp)

target_link_libraries(${exe_name} corelib argparse)

add_executable(${test_name} tests/wav_tests.cpp tests/config_parser_tests.cpp)
target_link_libraries(${test_name} gtest_main)
target_link_libraries(${test_name} corelib)
