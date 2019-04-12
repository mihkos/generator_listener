
#pragma once

#include "../includes.hpp"

class UDPGenerator{
public:
    UDPGenerator() = delete;
    UDPGenerator(const struct params& current_params);
    ~UDPGenerator();
    void start();

private:
    int this_socket;
    struct sockaddr_in d_addr;
    std::vector<char> testMessage;
    static std::vector<char> createTestMessage(std::string templ_msg);
};
