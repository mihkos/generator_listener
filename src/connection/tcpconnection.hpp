#pragma once

#include "connection.hpp"

struct TCPConnection: Connection {

    TCPConnection(uint16_t sport);
    TCPConnection(std::unique_ptr<Socket> connected_socket);
    void connect_client_to_server(const Endpoint& server_addr);
    uint16_t send(const uint8_t* data, size_t size) override;
    uint16_t receive(uint8_t* data, size_t size) override;
    ~TCPConnection() {
        _socket->shutdown();
    }
};