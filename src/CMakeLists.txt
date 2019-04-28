cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

set(SOURCES
        main.cpp
        generator/tcpgenerator.cpp
        generator/udpgenerator.cpp
        listener/tcplistener.cpp
        listener/udplistener.cpp)

find_library(PTHREAD_LIB pthread)

if(NOT PTHREAD_LIB)
    message(FATAL_ERROR "pthread library not found")
endif()

add_subdirectory(socket)

add_executable(main ${SOURCES})

target_include_directories(
        main
        PUBLIC
        listener
        generator)

target_link_libraries(main pthread socketlib)