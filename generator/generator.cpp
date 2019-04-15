

#include "generator.hpp"

UDPGenerator::UDPGenerator(const params& current_params) : Generator(current_params) {
    _testMessage = createTestMessage(current_params.test_message, UDP_LENGTH_DATAGRAM);
}

UDPGenerator::~UDPGenerator() {
    close(_this_socket);
}

void UDPGenerator::start() {
    std::thread sender([&] () {
        while(t_running) {
            if (sendto(_this_socket, &_testMessage[0], _testMessage.size(), 0, (sockaddr*)&_d_addr, sizeof(_d_addr)) < 0) {
                throw std::runtime_error("error sendto function socket");
            }
        }
    });
    std::thread receiver([&] () {
        char buf[UDP_LENGTH_DATAGRAM];
        int32_t bytes_read(0);
        while(t_running) {
            do {
                errno = 0;
                bytes_read = recvfrom(_this_socket, buf, UDP_LENGTH_DATAGRAM, 0, nullptr, nullptr);
            } while ((errno == EAGAIN) && (t_running != 0));
            buf[bytes_read] = '\0';
        }
    });
    std::cout << "Begining sending..." << std::endl;
    std::cout << "For stopping send input \"ctrl+c\": " << std::endl;
    sender.join();
    receiver.join();
    std::cout << "Sending stopped" << std::endl;
}

TCPGenerator::TCPGenerator(const params& current_params) : Generator(current_params) {
    _testMessage = createTestMessage(current_params.test_message, TCP_LENGTH_MSG);
}

void TCPGenerator::start() {
    if (connect(_this_socket , (struct sockaddr *)&_d_addr , sizeof(_d_addr)) < 0) {
        throw std::runtime_error("tcp connect function");
    }
    std::thread sender([&] () {
        char buf[TCP_LENGTH_MSG];
        int32_t bytes_read(0);
        while (t_running) {
            if (send(_this_socket, &_testMessage[0], _testMessage.size(), 0) < 0) {
                throw std::runtime_error("send function");
            }
            if(t_running) {
                if ((bytes_read = recv(_this_socket, buf, TCP_LENGTH_MSG, 0)) < 0) {
                    throw std::runtime_error("recv function");
                }
                if (bytes_read == 0) {
                    t_running = false;
                }
                buf[bytes_read] = '\0';
            }
        }
    });
    std::cout << "Begining sending..." << std::endl;
    std::cout << "For stopping send input \"ctrl+c\": " << std::endl;
    sender.join();
    std::cout << "Sending stopped" << std::endl;
}

TCPGenerator::~TCPGenerator()
{
    shutdown(_this_socket, SHUT_WR);
};