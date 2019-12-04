cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(generator_listener CXX)

set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
    message(STATUS "Default build configuration: Release")
endif()

if (CMAKE_BUILD_TYPE MATCHES Debug)
    set(CMAKE_REQUIRED_FLAGS -std=c++17 -Wall -g -O0)
endif()

if (CMAKE_BUILD_TYPE MATCHES Release)
    set(CMAKE_REQUIRED_FLAGS -std=c++17 -O3 -Wall -Wextra -pedantic)
endif()

add_subdirectory(src)

set_property(TARGET main PROPERTY CXX_STANDARD 17)


