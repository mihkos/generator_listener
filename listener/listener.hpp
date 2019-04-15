
#pragma once

#include "../includes.hpp"

class UDPListener : Listener {
public:
    UDPListener() = delete;
    UDPListener(const params& current_params);
    ~UDPListener();
    void start();

};

class TCPListener : Listener {
public:
    TCPListener() = delete;
    TCPListener(const params& current_params);
    ~TCPListener();
    void start();

private:
    std::vector<std::thread> _connetions;
};