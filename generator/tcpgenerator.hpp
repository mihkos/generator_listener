#pragma once

#include "generator.hpp"

#define TCP_LENGTH_MSG 16384

struct TCPGenerator : Generator {
    TCPGenerator() = delete;
    TCPGenerator(const params& current_params) : Generator(current_params) {
        _testMessage = createTestMessage(current_params._test_message, TCP_LENGTH_MSG);
    }
    ~TCPGenerator() = default;
    void start() {
        _socket.connect();
        std::thread sender([&] () {
            try {
                auto buf = std::make_unique<char[]>(TCP_LENGTH_MSG);
                while (t_running) {
                    _socket.send(&_testMessage[0], (int32_t)_testMessage.size());
                    int32_t bytes_read = _socket.recv(buf.get(), TCP_LENGTH_MSG);
                }
            }
            catch (const std::exception& error) {
                std::cout << "Server disconnected, reason: " << error.what() << std::endl;
            }
        });
        std::cout << "Begining sending..." << std::endl;
        std::cout << "For stopping send input \"ctrl+c\": " << std::endl;
        sender.join();
        std::cout << "Sending stopped" << std::endl;
    }
};