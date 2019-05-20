#include "clientconnectionmanager.hpp"

ClientConnectionManager::ClientConnectionManager(const json & j_config):
        _M_udp(j_config["number_udp_clients"]),
        _N_tcp(j_config["number_tcp_clients"]),
        _server_addr(j_config["dest_ip"]),
        _testMessage(createTestMessage(j_config["message"].get<std::string>(), length_test_message)) {
    uint16_t begin_sport = j_config["begin_sport"];
    uint16_t begin_dport = j_config["begin_dport"];
    uint16_t end_dport = j_config["end_dport"];
    uint16_t tcp_dport = j_config["tcp_dport"];
    for(uint32_t i = 0; i < _M_udp; ++i) {
        _udp_clients.emplace_back(client{._port = begin_sport, ._dest_addr = Endpoint(_server_addr, begin_dport)});
        ++begin_dport;
        ++begin_sport;
    }
    for (uint32_t j = 0; j < _N_tcp; ++j) {
        _tcp_clients.emplace_back(client{._port = begin_sport, ._dest_addr = Endpoint(_server_addr, tcp_dport)});
        ++begin_dport;
        ++begin_sport;
    }
}

void ClientConnectionManager::start() {
    std::unique_ptr<Connection> new_connection;
    for(uint32_t i = 0; i < _M_udp; i++)
    {
        new_connection.reset(new UDPConnection(_udp_clients[i]._port, Endpoint(_udp_clients[i]._dest_addr)));
        run_connection(std::move(new_connection));
    }
    for(uint32_t i = 0; i < _N_tcp; i++)
    {
        auto tcp_connect = std::make_unique<TCPConnection>(_tcp_clients[i]._port);
        tcp_connect->connect_client_to_server(Endpoint(_tcp_clients[i]._dest_addr));
        new_connection = std::move(tcp_connect);
        run_connection(std::move(new_connection));
    }
}

void ClientConnectionManager::do_run_connection(std::unique_ptr<Connection> connection, std::shared_ptr<bool> is_running) {
    uint8_t buf[length_test_message];
    while (*(is_running.get())){
        try {
            auto bytes_sent = connection->send(&_testMessage[0], _testMessage.size());
            statistics._bytes_sent += bytes_sent;
            auto bytes_read = connection->receive(buf, length_test_message);
            statistics._bytes_received += bytes_read;
        }
        catch (const std::system_error& error) {
            if(error.code().value() != EAGAIN) {
                throw error;
            }
        }
    }
}