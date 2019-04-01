cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(ListenerLibrary)

set(SOURCES listener.cpp)
set(HEADERS listener.h)

find_library(PTHREAD_LIB pthread)
if(NOT PTHREAD_LIB)
    message(FATAL_ERROR "pthread library not found")
endif()

add_library(listener_lib ${SOURCES} ${HEADERS})
target_link_libraries(listener_lib pthread)
target_include_directories(listener_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})