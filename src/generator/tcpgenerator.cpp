#include "tcpgenerator.hpp"

void TCPGenerator::start() {
    Generator::start();
    for(auto& socket : _sockets) {
        socket.connect();
        std::cout << "Create new client on port: " << socket.getSPort() << std::endl;
        _connections.emplace_back(std::thread([&, socket = std::move(socket)] () mutable {
            try {
                auto buf = std::make_unique<char[]>(TCP_LENGTH_MSG);
                while (t_running) {
                    socket.send(&_testMessage[0], (int32_t)_testMessage.size());
                    uint32_t bytes_read = socket.recv(buf.get(), TCP_LENGTH_MSG);
                    _received_bytes += bytes_read;
                }
            }
            catch (const std::exception& error) {
                t_running = false;
                std::cout << "Server disconnected, reason: " << error.what() << std::endl;
            }
        }));
    }
    std::cout << "Begining sending..." << std::endl;
    std::cout << "For stopping send input \"ctrl+c\": " << std::endl;
    for(auto& connection : _connections) {
        connection.join();
    }
    std::cout << std::endl <<"Sending stopped" << std::endl;
}
