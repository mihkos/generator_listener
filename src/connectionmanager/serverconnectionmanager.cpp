#include "serverconnectionmanager.hpp"

ServerConnectionManager::ServerConnectionManager(const json& j_config) :
        _tcp_acceptor(j_config["tcp_port"], j_config["backlog"])
{
    _start_udp_port = j_config["udp_port_X"];
    _end_udp_port = j_config["udp_port_Y"];
}
void ServerConnectionManager::start() {
    _tcp_acceptor.start(this);
    std::unique_ptr<Connection> new_connection;
    for(uint16_t i = 0; i <= _end_udp_port - _start_udp_port; ++i) {
        new_connection = std::make_unique<UDPConnection>(_start_udp_port + i, Endpoint("", 0));
        run_connection(std::move(new_connection));
    }
}

void ServerConnectionManager::stop() {
    _tcp_acceptor.stop();
    ConnectionManager::stop();
}

void ServerConnectionManager::do_run_connection(std::unique_ptr<Connection> connection, std::shared_ptr<bool> is_running) {
    uint8_t buf[length_test_message];
    while (*(is_running.get())) {
        try {
            connection->doServerConnection(buf, length_test_message);
        }
        catch (const std::system_error& error) {
            if(error.code().value() != EAGAIN) {
                throw error;
            }
        }

    }
}
