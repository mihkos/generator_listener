cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(udp_generator_listener)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

FIND_PACKAGE( Boost 1.40 COMPONENTS thread REQUIRED )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR})

set(SOURCES main.cpp generator/generator.cpp listener/listener.cpp)
set(HEADERS includes.hpp generator/generator.hpp listener/listener.hpp)

add_executable(main ${SOURCES} ${HEADERS})

target_include_directories(main PUBLIC generator)
target_include_directories(main PUBLIC listener)

target_link_libraries( main ${Boost_LIBRARIES} )
