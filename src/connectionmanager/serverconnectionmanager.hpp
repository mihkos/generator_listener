#pragma once

#include "connectionmanager.hpp"
#include "tcpconnection.hpp"
#include "udpconnection.hpp"
#include "tcpacceptor.hpp"

struct ServerConnectionManager: ConnectionManager {
    ServerConnectionManager(const json& j_config);
    void start() override;
    void stop() override;

private:
    void do_run_connection(std::unique_ptr<Connection> connection, std::shared_ptr<bool> is_running) override;
    TCPAcceptor _tcp_acceptor;
    uint16_t _start_udp_port;
    uint16_t _end_udp_port;
};