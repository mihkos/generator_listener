#pragma once

#include "listener.hpp"

struct UDPListener : Listener {
    UDPListener() = delete;
    UDPListener(const params& current_params) : Listener(current_params) {}
    ~UDPListener() = default;
    void start() override;

};
