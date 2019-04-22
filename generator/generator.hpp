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
        _socket.bind();
        _socket.setPartner(current_params);
    }
    Generator(const Generator&) = delete;
    Generator& operator=(const Generator&) = delete;
    virtual void start() = 0;
    virtual ~Generator() = default;
protected:
    Socket _socket;
    std::vector<char> _testMessage;
    static std::vector<char> createTestMessage(std::string templ_msg, size_t length_result_msg)
    {
        std::vector<char> res_test_msg;
        auto size_test_message = templ_msg.length();
        while(res_test_msg.size() + size_test_message < length_result_msg) {
            std::copy(templ_msg.begin(), templ_msg.end(), std::back_inserter(res_test_msg));
        }
        std::copy(templ_msg.c_str(), templ_msg.c_str()+(length_result_msg - res_test_msg.size()), std::back_inserter(res_test_msg));
        return res_test_msg;
    }
};



