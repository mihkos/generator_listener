cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

include(ExternalProject)

set(SOURCES
        main.cpp
        socket/socket.cpp
        endpoint/endpoint.cpp
        connectionmanager/tcpacceptor.cpp
        connectionmanager/serverconnectionmanager.cpp
        connectionmanager/clientconnectionmanager.cpp
        connectionmanager/connectionmanager.cpp
        connection/tcpconnection.cpp
        connection/udpconnection.cpp
)

find_library(PTHREAD_LIB pthread)

if(NOT PTHREAD_LIB)
    message(FATAL_ERROR "pthread library not found")
endif()

add_executable(main ${SOURCES})
target_include_directories(
        main
        PUBLIC
        socket
        endpoint
        connection
        connectionmanager)
target_link_libraries(main PUBLIC pthread)

ExternalProject_Add(
        json
        URL "${CMAKE_CURRENT_SOURCE_DIR}/../externals/nlohmann_json.zip"
        PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/external/json"
        CMAKE_ARGS -DJSON_BuildTests=OFF -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
)

ExternalProject_Get_Property(json BINARY_DIR)
ExternalProject_Get_Property(json SOURCE_DIR)

target_include_directories(
        main
        PUBLIC
        ${SOURCE_DIR}/include)

target_link_libraries(main PUBLIC ${BINARY_DIR})

ExternalProject_Add(
        clipp
        URL "${CMAKE_CURRENT_SOURCE_DIR}/../externals/clipp.zip"
        PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/external/clipp"
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
)
ExternalProject_Get_Property(clipp BINARY_DIR)
ExternalProject_Get_Property(clipp SOURCE_DIR)

target_include_directories(
        main
        PUBLIC
        ${SOURCE_DIR}/include)

target_link_libraries(main PUBLIC ${BINARY_DIR})
