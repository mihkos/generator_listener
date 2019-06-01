#pragma once

#include "connection.hpp"

struct UDPConnection: Connection {
    UDPConnection(uint16_t sport, const Endpoint& partner_addr);
    ~UDPConnection() {
       if(_receiver.joinable()) {
           _receiver.join();
       }
    }
    void setPartner(const Endpoint& endpoint);
    uint16_t send(const uint8_t* data, size_t size) override;
    uint16_t receive(uint8_t* data, size_t size) override;
    void startReceiver();
    void doClientConnection(const uint8_t* outdata, size_t outsize, uint8_t* indata, size_t insize) override;
    void doServerConnection(uint8_t* inOutData, size_t insize) override;
private:
    Endpoint _partner_addr;
    std::thread _receiver;
};