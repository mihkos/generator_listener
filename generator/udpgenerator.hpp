#pragma once

#include "generator.hpp"
#define UDP_LENGTH_DATAGRAM 16384

struct UDPGenerator : Generator {
    UDPGenerator() = delete;
    UDPGenerator(const params& current_params) : Generator(current_params) {
        _testMessage = createTestMessage(current_params._test_message, UDP_LENGTH_DATAGRAM);
    }
    ~UDPGenerator() = default;
    void start() {
        std::thread sender([&] () {
            while(t_running) {
                _socket.sendto(&_testMessage[0], (int32_t )_testMessage.size());
            }
        });
        std::thread receiver([&] () {
            auto buf = std::make_unique<char[]>(UDP_LENGTH_DATAGRAM);
            while(t_running) {
                auto bytes_read = _socket.recvfrom(buf.get(), UDP_LENGTH_DATAGRAM, nullptr);
            }
        });
        std::cout << "Begining sending..." << std::endl;
        std::cout << "For stopping send input \"ctrl+c\": " << std::endl;
        sender.join();
        receiver.join();
        std::cout << "Sending stopped" << std::endl;
    }
};