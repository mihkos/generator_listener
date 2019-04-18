#pragma once

#include "generator.hpp"
#define UDP_LENGTH_DATAGRAM 16384

struct UDPGenerator : Generator{
    UDPGenerator() = delete;
    UDPGenerator(const params& current_params) : Generator(current_params) {
        _testMessage = createTestMessage(current_params._test_message, UDP_LENGTH_DATAGRAM);
    }
    ~UDPGenerator() = default;
    void start() {
        std::thread sender([&] () {
            while(t_running) {
                if (sendto(_socket.get(), &_testMessage[0], _testMessage.size(), 0, (sockaddr*)&_d_addr, sizeof(_d_addr)) < 0) {
                    throw std::runtime_error("error sendto function socket");
                }
            }
        });
        std::thread receiver([&] () {
            auto buf = std::make_unique<char[]>(UDP_LENGTH_DATAGRAM);
            int32_t bytes_read(0);
            while(t_running) {
                errno = 0;
                bytes_read = recvfrom(_socket.get(), buf.get(), UDP_LENGTH_DATAGRAM, 0, nullptr, nullptr);
            }
        });
        std::cout << "Begining sending..." << std::endl;
        std::cout << "For stopping send input \"ctrl+c\": " << std::endl;
        sender.join();
        receiver.join();
        std::cout << "Sending stopped" << std::endl;
    }
};