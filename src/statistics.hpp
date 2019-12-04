#pragma once

#include <atomic>
#include <iostream>
#include <thread>
extern volatile bool main_is_running;
struct Statistics {
    Statistics() : _bytes_received(0), _bytes_sent(0) {
        _speedmeter = std::thread([&] () {
            double one_mbt = 1024 * 128;
            uint64_t last_seen_bytes_received_(0);
            auto start = std::chrono::system_clock::now();
            while(main_is_running) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                auto end = std::chrono::system_clock::now();
                double elapsed_seconds = std::chrono::duration<double>(end - start).count();
                res_download_speed = (_bytes_received - last_seen_bytes_received_) / one_mbt / elapsed_seconds;
                last_seen_bytes_received_ = _bytes_received;
                start = std::chrono::system_clock::now();
            }
        });
    }
    ~Statistics() {
        _speedmeter.join();
    }
    std::atomic<size_t> _bytes_received;
    std::atomic<size_t> _bytes_sent;

    void print() {
        std::cout << "Download speed: " <<  std::fixed << std::setw(10) << std::setprecision(3) << res_download_speed << " Mbps\r";
        std::cout.flush();
    }
private:
    std::thread _speedmeter;
    std::atomic<double> res_download_speed;
};