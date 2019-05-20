#include "tcpacceptor.hpp"

TCPAcceptor::TCPAcceptor(uint16_t sport, uint32_t backlog = 5) {
    _acceptor = std::make_unique<Socket>(AF_INET, SOCK_STREAM, 0);
    _acceptor->bind(sport);
    timeval tv = {RECV_TIMEOUT_SECS, RECV_TIMEOUT_USECS};
    _acceptor->setsockopt(SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    int32_t enable = 1;
    _acceptor->setsockopt(SOL_SOCKET, SO_REUSEADDR,  &enable, sizeof(enable));
    _acceptor->listen(backlog);
}

void TCPAcceptor::start(ConnectionManager* connectionManager) {
    thread = std::thread([=] {
        try {
            run(connectionManager);
        }
        catch (const std::exception& error) {
            std::cout << "Caught: " << error.what() << std::endl;
            std::terminate();
        }
    });
}

void TCPAcceptor::stop() {
    thread.join();
}

void TCPAcceptor::run(ConnectionManager* connectionManager) {
    while (main_is_running) {
        try {
            auto accepted_client = std::make_unique<Endpoint>("", 0);
            Socket socket = _acceptor->accept(accepted_client.get());
            connectionManager->run_connection(std::make_unique<TCPConnection>(std::make_unique<Socket>(std::move(socket))));
        }
        catch (const std::exception& error) {
            if(errno != EAGAIN) {
                throw error;
            }
        }
    }
}