cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(udp_generator_listener)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

#add_library(generator_lib STATIC generator/generator.cpp)
#add_library(listener_lib STATIC listener/listener.cpp)

add_executable(main main.cpp)

add_subdirectory(generator)
add_subdirectory(listener)

#target_link_libraries(listener_lib pthread)
target_link_libraries(main listener_lib)
target_link_libraries(main generator_lib)