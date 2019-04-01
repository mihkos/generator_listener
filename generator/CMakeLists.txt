cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(GeneratorLibrary)

set(SOURCES generator.cpp)
set(HEADERS generator.h)

add_library(generator_lib ${SOURCES} ${HEADERS})
target_include_directories(generator_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})