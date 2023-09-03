cmake_minimum_required(VERSION 3.10)

project(Game2048)

set(CMAKE_CXX_STANDARD 11)

include_directories(src)
add_executable(Game2048 src/main.cpp src/Game2048.cpp)
