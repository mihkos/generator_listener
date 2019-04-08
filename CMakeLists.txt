cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(udp_generator_listener CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_REQUIRED_FLAGS -std=c++17)

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
