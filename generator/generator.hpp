#pragma once

#include <netinet/in.h>
#include <vector>
#include <string>
#include <arpa/inet.h>

#include "../params.hpp"
#include "../socket.hpp"

extern volatile bool t_running;

class Generator {
public:
    Generator() = default;
    Generator(const params& current_params) : _socket(current_params) {
        _d_addr.sin_family = AF_INET;
        inet_pton(AF_INET, current_params._dest_ip.c_str(), &(_d_addr.sin_addr));
        _d_addr.sin_port = htons(current_params._dport);
    }
    virtual void start() = 0;
    virtual ~Generator() = default;
protected:
    Socket _socket;
    sockaddr_in _d_addr;
    std::vector<char> _testMessage;
    static std::vector<char> createTestMessage(std::string templ_msg, size_t length_result_msg)
    {
        std::vector<char> res_test_msg;
        size_t counter = 0;
        auto size_test_message = templ_msg.length();
        while(counter + size_test_message < length_result_msg) {
            std::copy(templ_msg.begin(), templ_msg.end(), std::back_inserter(res_test_msg));
            counter += size_test_message;
        }
        std::copy(templ_msg.c_str(), templ_msg.c_str()+(length_result_msg - counter), std::back_inserter(res_test_msg));
        return res_test_msg;
    }
};



