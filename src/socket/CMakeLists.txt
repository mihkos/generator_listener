cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

add_library(socketlib socket.cpp)

target_include_directories(socketlib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})