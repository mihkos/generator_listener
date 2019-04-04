
#pragma once

#include "../includes.hpp"
class UDPListener
{
public:
    UDPListener(unsigned short listen_port);
    ~UDPListener();
    void startListener();

private:
    int this_socket;
    std::atomic<bool> t_running;
    void handle(std::atomic<long>* received_bytes);
    void calculatingSpeed(std::atomic<long>* received_bytes);
};

