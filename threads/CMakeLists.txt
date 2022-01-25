cmake_minimum_required(VERSION 3.15)
project(threads C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_FLAGS "-Wall -pthread")
add_executable(threads main.c)
