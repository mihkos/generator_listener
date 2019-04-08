
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
    void handle(std::atomic<long>* received_bytes);
    void calculatingSpeed(std::atomic<long>* received_bytes);
};

