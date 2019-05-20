#pragma once

#include "connectionmanager.hpp"
#include "tcpconnection.hpp"

struct TCPAcceptor {
    TCPAcceptor(uint16_t sport, uint32_t backlog);
    ~TCPAcceptor() = default;
    void start(ConnectionManager* connectionManager);
    void stop();

private:
    void run(ConnectionManager* connectionManager);

    std::unique_ptr<Socket> _acceptor;
    std::thread thread;
};