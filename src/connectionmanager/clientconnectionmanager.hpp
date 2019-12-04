#pragma once

#include "connectionmanager.hpp"
#include "tcpconnection.hpp"
#include "udpconnection.hpp"


struct client {
    uint16_t _port;
    Endpoint _dest_addr;
};
struct ClientConnectionManager: ConnectionManager {
    ClientConnectionManager(const json & j_config);
    void start() override;
    static std::vector<uint8_t> createTestMessage(std::string templ_msg, size_t length_result_msg)
    {
        std::vector<uint8_t> res_test_msg;
        auto size_test_message = templ_msg.length();
        while(res_test_msg.size() + size_test_message < length_result_msg) {
            std::copy(templ_msg.begin(), templ_msg.end(), std::back_inserter(res_test_msg));
        }
        std::copy(templ_msg.c_str(), templ_msg.c_str()+(length_result_msg - res_test_msg.size()), std::back_inserter(res_test_msg));
        return res_test_msg;
    }

private:
    uint32_t _M_udp = 0;
    uint32_t _N_tcp = 0;
    std::vector<client> _udp_clients;
    std::vector<client> _tcp_clients;
    std::string _server_addr;
    std::vector<uint8_t> _testMessage;
    void do_run_connection(std::unique_ptr<Connection> connection, std::shared_ptr<bool> is_running) override;
};