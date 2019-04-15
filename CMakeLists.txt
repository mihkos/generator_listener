cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(generator_listener CXX)

set(CMAKE_CXX_STANDARD_REQUIRED on)

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(CMAKE_REQUIRED_FLAGS -std=c++17 -Wall)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/debug/lib/)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/debug/lib/)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/debug/bin/)
endif()
if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
    set(CMAKE_REQUIRED_FLAGS -std=c++17 -Wall -Wextra)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/release/lib/)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/release/lib/)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/release/bin/)
endif()

find_library(PTHREAD_LIB pthread)
if(NOT PTHREAD_LIB)
    message(FATAL_ERROR "pthread library not found")
endif()

set(SOURCES main.cpp generator/generator.cpp listener/listener.cpp)


add_executable(main ${SOURCES})
set_property(TARGET main PROPERTY CXX_STANDARD 17)
target_include_directories(main PUBLIC generator)
target_include_directories(main PUBLIC listener)

target_link_libraries(main pthread )
