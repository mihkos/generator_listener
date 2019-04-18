#pragma once

//#include "../includes.hpp"
#include <thread>
#include <iostream>
#include <iomanip>
#include "../params.hpp"
#include "../socket.hpp"



extern volatile bool t_running;


class Listener {
public:
    Listener() = default;
    Listener(const params& current_params): _socket(current_params), _received_bytes(0) {
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
    virtual void start() = 0;
    virtual ~Listener() {
        _speedmeter.join();
    }
protected:
    Socket _socket;
    uint64_t _received_bytes;
    std::thread _speedmeter;
};

