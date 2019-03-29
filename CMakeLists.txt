cmake_minimum_required(VERSION 3.1)

project(udp_generator_listener)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED on)

add_library(libgenerator STATIC generator/generator.cpp)
add_library(liblistener STATIC listener/listener.cpp)

add_executable(main main.cpp)

target_link_libraries(main liblistener)
target_link_libraries(main libgenerator)