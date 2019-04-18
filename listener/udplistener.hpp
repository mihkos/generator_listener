#pragma once

#include "listener.hpp"

#define UDP_LENGTH_DATAGRAM 16384

struct UDPListener : Listener {
    UDPListener() = delete;
    UDPListener(const params& current_params) : Listener(current_params) {}
    ~UDPListener() = default;
    void start() {

        std::thread receiver([&] () {
            auto buf = std::make_unique<char[]>(UDP_LENGTH_DATAGRAM);
            int32_t bytes_read(0);
            sockaddr_in client_addr;
            socklen_t client_addr_len(sizeof(client_addr));
            while(t_running) {
                do {
                    errno = 0;
                    bytes_read = recvfrom(_socket.get(), buf.get(), UDP_LENGTH_DATAGRAM, 0, (sockaddr *)&client_addr, &client_addr_len);
                } while ((errno == EAGAIN) && (t_running != 0));
                _received_bytes += bytes_read;

                if (sendto(_socket.get(), buf.get(), (size_t)bytes_read, 0, (sockaddr *) &client_addr, sizeof(client_addr)) < 0) {
                    throw std::runtime_error("error sendto function socket");
                }
            }
        });
        std::cout << "Listening..." << std::endl;
        std::cout << "For stopping listen input \"ctrl+c\": " << std::endl;
        receiver.join();
        std::cout << std::endl << "Listening stopped" << std::endl;
    }

};
