#pragma once

#include "listener.hpp"
#include <arpa/inet.h>

#define TCP_LENGTH_MSG 16384
#define QUEUE_SIZE_PENDING_CONNECTIONS 5

struct TCPListener : Listener {
    TCPListener() = delete;
    TCPListener(const params& current_params) : Listener(current_params) {}
    ~TCPListener() = default;
    void start() {

        if(listen(_socket.get(), QUEUE_SIZE_PENDING_CONNECTIONS) < 0) {
            throw std::runtime_error("tcp listen function");
        }

        std::thread receiver([&] () {
            auto ip_addr = std::make_unique<char[]>(INET_ADDRSTRLEN);
            sockaddr_in client_addr;
            socklen_t client_addr_len(sizeof(client_addr));
            fd_set set;
            while(t_running) {
                FD_ZERO(&set);
                FD_SET(_socket.get(), &set);
                timeval timeout = {0, 0};
                auto count = select(_socket.get() + 1, &set, 0, 0, &timeout);
                if (count) {
                    Socket connection(accept(_socket.get(), (sockaddr *) &client_addr, &client_addr_len));
                    if (connection.get() < 0) {
                        throw std::runtime_error("tcp accept function");
                    }
                    inet_ntop(AF_INET, &(client_addr.sin_addr), ip_addr.get(), INET_ADDRSTRLEN);
                    std::cout << std::endl << "Connected client: " << ip_addr.get() << ":"
                              << ntohs(client_addr.sin_port) << std::endl;

                    auto buf = std::make_unique<char[]>(TCP_LENGTH_MSG);
                    int32_t bytes_read(0);
                    while (t_running) {
                        bytes_read = recv(connection.get(), buf.get(), TCP_LENGTH_MSG, 0);
                        if (bytes_read == 0) {
                            std::cout << "Close connect" << std::endl;
                            break;
                        }
                        _received_bytes += bytes_read;

                        if (send(connection.get(), buf.get(), (size_t) bytes_read, 0) < 0) {
                            break;
                        }
                    }
                }
            }
        });
        std::cout << "Listening..." << std::endl;
        std::cout << "For stopping listen input \"ctrl+c\": " << std::endl;
        receiver.join();
        std::cout << std::endl << "Listening stopped" << std::endl;
    }

};