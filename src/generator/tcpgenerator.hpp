#pragma once

#include "generator.hpp"

#define TCP_LENGTH_MSG 16384

struct TCPGenerator : Generator {
    TCPGenerator() = delete;

    TCPGenerator(const params &current_params) : Generator(current_params) {
        _testMessage = createTestMessage(current_params._test_message, TCP_LENGTH_MSG);
        _sockets.reserve(current_params._number_sessions);
        _sockets.emplace_back(std::move(_socket));
        for(auto number_sessions = 1; number_sessions < current_params._number_sessions; ++number_sessions) {
            Socket socket(current_params);
            socket.resetSPort(socket.getSPort() + number_sessions);
            socket.bind();
            socket.setPartner(current_params);
            _sockets.emplace_back(std::move(socket));
        }
    }

    ~TCPGenerator() = default;

    void start() override;
private:
    std::vector<Socket> _sockets;
    std::vector<std::thread> _connections;
};