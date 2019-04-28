#include "udplistener.hpp"

#define UDP_LENGTH_DATAGRAM 16384

void UDPListener::start() {
    Listener::start();
    std::thread receiver([&] () {
        try {
            auto buf = std::make_unique<char[]>(UDP_LENGTH_DATAGRAM);
            while(t_running) {
                sockaddr_in client_addr;

                auto bytes_read = _socket.recvfrom(buf.get(), UDP_LENGTH_DATAGRAM, &client_addr);

                _received_bytes += bytes_read;

                _socket.setPartner(client_addr);
                _socket.sendto(buf.get(), bytes_read);
            }
        }
        catch (const std::exception& error) {
            std::cout << error.what();
        }

    });
    std::cout << "Listening..." << std::endl;
    std::cout << "For stopping listen input \"ctrl+c\": " << std::endl;
    receiver.join();
    std::cout << std::endl << "Listening stopped" << std::endl;
}