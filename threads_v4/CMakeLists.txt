cmake_minimum_required(VERSION 3.16)
project(threads_v4 C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-pthread -Wall")
add_executable(threads_v4 main.c)
