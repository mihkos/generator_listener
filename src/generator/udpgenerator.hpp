#pragma once

#include "generator.hpp"
#define UDP_LENGTH_DATAGRAM 16384

struct UDPGenerator : Generator {
    UDPGenerator() = delete;

    UDPGenerator(const params &current_params) : Generator(current_params) {
        _testMessage = createTestMessage(current_params._test_message, UDP_LENGTH_DATAGRAM);
    }

    ~UDPGenerator() = default;

    void start() override;
};