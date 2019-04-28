#pragma once

#include <netinet/in.h>
#include <vector>
#include <string>
#include <arpa/inet.h>
#include <thread>
#include <atomic>

#include "../params.hpp"
#include "socket.hpp"

extern volatile bool t_running;

class Generator {
public:
    Generator() = default;
    Generator(const params& current_params) : _socket(current_params), _received_bytes(0) {
        _socket.bind();
        _socket.setPartner(current_params);
    }
    Generator(const Generator&) = delete;
    Generator& operator=(const Generator&) = delete;
    virtual void start() {
        _speedmeter = std::thread([&] () {
            double one_mbt = 1024 * 128;
            uint64_t last_seen_received_bytes(0);
            auto start = std::chrono::system_clock::now();
            while(t_running) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                auto end = std::chrono::system_clock::now();
                double elapsed_seconds = std::chrono::duration<double>(end - start).count();
                double res_speed = (_received_bytes - last_seen_received_bytes) / one_mbt / elapsed_seconds;
                last_seen_received_bytes = _received_bytes;
                std::cout << "speed: " <<  std::fixed << std::setw(10) << std::setprecision(3) << res_speed << " Mbps\r";
                std::cout.flush();
                start = std::chrono::system_clock::now();
            }
        });
    }
    virtual ~Generator()
    {
        _speedmeter.join();
    }
protected:
    Socket _socket;
    std::vector<char> _testMessage;
    std::atomic<uint64_t> _received_bytes;
    std::thread _speedmeter;
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



