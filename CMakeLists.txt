cmake_minimum_required(VERSION 3.30)

project(circular_buffer)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(circular_buffer src/circular_buffer.c)

add_executable(circular_buffer_test src/circular_buffer_test.c)
target_link_libraries(circular_buffer_test circular_buffer)
