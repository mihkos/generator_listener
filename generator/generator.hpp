
#pragma once

#include "../includes.hpp"

class UDPGenerator : Generator{
public:
    UDPGenerator() = delete;
    UDPGenerator(const params& current_params);
    ~UDPGenerator();
    void start();
};

class TCPGenerator : Generator{
public:
    TCPGenerator() = delete;
    TCPGenerator(const params& current_params);
    ~TCPGenerator();
    void start();
};