#pragma once

#include "listener.hpp"
#include <arpa/inet.h>
#include <map>

struct TCPListener : Listener {

    TCPListener() = delete;
    TCPListener(const params& current_params) : Listener(current_params) {}
    ~TCPListener() = default;
    void updateConnections();
    void closeConnections();
    void start() override;
private:
    std::map<int32_t, std::thread>_connections;
    std::map<int32_t, bool> _states_connections;
};