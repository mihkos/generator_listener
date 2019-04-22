#pragma once

#include "listener.hpp"
#include <arpa/inet.h>

#define TCP_LENGTH_MSG 16384

struct TCPListener : Listener {
    TCPListener() = delete;
    TCPListener(const params& current_params) : Listener(current_params) {}
    ~TCPListener() = default;
    void start() override {
        Listener::start();
        _socket.listen();
        std::thread receiver([&] () {
            while(t_running) {
                Socket connection = _socket.accept();
                std::cout << "Connected client: " << connection.peername() << std::endl;
                try {
                    std::unique_ptr<char[]> buf = std::make_unique<char[]>(TCP_LENGTH_MSG);
                    while (t_running) {
                        int32_t bytes_read = connection.recv(buf.get(), TCP_LENGTH_MSG);
                        _received_bytes += bytes_read;
                        connection.send(buf.get(), bytes_read);
                    }
                }
                catch (const std::exception& error) {
                    std::cout << "Client disconnected, reason: " << error.what() << std::endl;
                }
            }
        });
        std::cout << "Listening..." << std::endl;
        std::cout << "For stopping listen input \"ctrl+c\": " << std::endl;
        receiver.join();
        std::cout << std::endl << "Listening stopped" << std::endl;
    }

};