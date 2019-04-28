#include "udpgenerator.hpp"

void UDPGenerator::start() {
    Generator::start();
    std::thread sender([&] () {
        while(t_running) {
            _socket.sendto(&_testMessage[0], (int32_t )_testMessage.size());
        }
    });
    std::thread receiver([&] () {
        auto buf = std::make_unique<char[]>(UDP_LENGTH_DATAGRAM);
        while(t_running) {
            auto bytes_read = _socket.recvfrom(buf.get(), UDP_LENGTH_DATAGRAM, nullptr);
            _received_bytes += bytes_read;
        }
    });
    std::cout << "Begining sending..." << std::endl;
    std::cout << "For stopping send input \"ctrl+c\": " << std::endl;
    sender.join();
    receiver.join();
    std::cout << std::endl << "Sending stopped" << std::endl;
}