cmake_minimum_required(VERSION 3.20)
project(game_of_life)
set(CMAKE_CXX_STANDARD 17)
set(GOOGLETEST_VERSION 1.14.0)

add_subdirectory(googletest/googletest)

enable_testing()

add_executable(game_of_life main.cpp view/view.h view/view.cpp model/model.h model/model.cpp controller/controller.h controller/controller.cpp
)

target_link_libraries(game_of_life gtest_main)