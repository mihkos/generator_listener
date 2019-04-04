
#pragma once

#include "../includes.hpp"

class UDPGenerator{
public:
    UDPGenerator();
    UDPGenerator(unsigned short sport);
    ~UDPGenerator();
    void startGenerator();
    void setDestAddr(std::string dest_ip, unsigned short dport);
    std::string testMessage;
private:
    int this_socket;
    struct sockaddr_in d_addr;
    std::atomic<bool> t_running;
    void handle();
    int fillTestMessage(char * msg);
};
