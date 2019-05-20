#pragma once

#include "connection.hpp"

struct UDPConnection: Connection {
    UDPConnection(uint16_t sport, const Endpoint& partner_addr);
    ~UDPConnection() = default;
    void setPartner(const Endpoint& endpoint);
    uint16_t send(const uint8_t* data, size_t size) override;
    uint16_t receive(uint8_t* data, size_t size) override;
private:
    Endpoint _partner_addr;
};