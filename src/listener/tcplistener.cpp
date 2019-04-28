#include "tcplistener.hpp"

#define TCP_LENGTH_MSG 16384

void TCPListener::closeConnections() {
    for(auto& status : _states_connections) {
        status.second = false;
    }
    for(auto& session : _connections) {
        session.second.join();
    }
}

void TCPListener::updateConnections() {
    auto end_iter = _states_connections.end();
    for(auto iter = _states_connections.begin(); iter != end_iter;) {
        if(!(iter->second)) {
            auto completed_session = _connections.find(iter->first);
            completed_session->second.join();
            _connections.erase(iter->first);
            _states_connections.erase(iter++);
        }
        else { ++iter; }
    }
}

void TCPListener::start() {
    Listener::start();
    _socket.listen();
    std::thread receiver([&] () {
        while(t_running) {
            auto connection = _socket.accept();
            updateConnections();
            std::cout << "Connected client: " << connection.peername() << std::endl;
            _states_connections.insert({connection.get(), true});
            _connections.insert({connection.get(), std::thread([&, connection = std::move(connection)] () mutable {
                try {
                    std::unique_ptr<char[]> buf = std::make_unique<char[]>(TCP_LENGTH_MSG);
                    while (_states_connections[connection.get()]) {
                        int32_t bytes_read = connection.recv(buf.get(), TCP_LENGTH_MSG);
                        this->_received_bytes += bytes_read;
                        connection.send(buf.get(), (uint32_t )bytes_read);
                    }
                }
                catch (const std::exception &error) {
                    _states_connections[connection.get()] = false;
                    std::cout << "Client disconnected, reason: " << error.what() << std::endl;
                }
            })});
        }
        closeConnections();
    });
    std::cout << "Listening..." << std::endl;
    std::cout << "For stopping listen input \"ctrl+c\": " << std::endl;
    receiver.join();
    std::cout << std::endl << "Listening stopped" << std::endl;
}

