
#pragma once

#include "../includes.hpp"

class UDPGenerator{
public:
    UDPGenerator() = delete;
    UDPGenerator(const struct params& current_params);
    ~UDPGenerator();
    void start();
    std::string testMessage;
private:
    int this_socket;
    struct sockaddr_in d_addr;
    size_t fillTestMessage(char * msg);
};
