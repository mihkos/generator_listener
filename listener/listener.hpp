
#pragma once

#include "../includes.hpp"

class UDPListener
{
public:
    UDPListener() = delete;
    UDPListener(const struct params& current_params);
    ~UDPListener();
    void start();

private:
    int32_t this_socket;
};

