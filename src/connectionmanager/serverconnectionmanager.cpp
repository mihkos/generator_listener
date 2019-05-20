#include "serverconnectionmanager.hpp"

ServerConnectionManager::ServerConnectionManager(const json& j_config) :
        _tcp_acceptor(j_config["tcp_sport"], j_config["backlog"])
{
    _start_udp_port = j_config["start_sport"];
    _end_udp_port = j_config["end_sport"];
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
            auto bytes_read = connection->receive(buf, length_test_message);
            statistics._bytes_received += bytes_read;
            auto bytes_sent = connection->send(buf, bytes_read);
            statistics._bytes_sent += bytes_sent;
        }
        catch (const std::system_error& error) {
            if(error.code().value() != EAGAIN) {
                throw error;
            }
        }

    }
}
