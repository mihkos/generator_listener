#pragma once

#include "generator.hpp"

#define TCP_LENGTH_MSG 16384

struct TCPGenerator : Generator{
    TCPGenerator() = delete;
    TCPGenerator(const params& current_params) : Generator(current_params) {
        _testMessage = createTestMessage(current_params._test_message, TCP_LENGTH_MSG);
    }
    ~TCPGenerator() = default;
    void start() {
        if (connect(_socket.get() , (struct sockaddr *)&_d_addr , sizeof(_d_addr)) < 0) {
            throw std::runtime_error("tcp connect function");
        }
        std::thread sender([&] () {
            auto buf = std::make_unique<char[]>(TCP_LENGTH_MSG);
            int32_t bytes_read(0);
            while (t_running) {
                if (send(_socket.get(), &_testMessage[0], _testMessage.size(), 0) < 0) {
                    throw std::runtime_error("send function");
                }
                if ((bytes_read = recv(_socket.get(), buf.get(), TCP_LENGTH_MSG, 0)) < 0) {
                    throw std::runtime_error("recv function");
                }
                if (bytes_read == 0) {
                    t_running = false;
                }
            }
        });
        std::cout << "Begining sending..." << std::endl;
        std::cout << "For stopping send input \"ctrl+c\": " << std::endl;
        sender.join();
        std::cout << "Sending stopped" << std::endl;
    }
};