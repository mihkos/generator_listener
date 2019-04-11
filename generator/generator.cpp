

#include "generator.hpp"

UDPGenerator::UDPGenerator(const struct params& current_params) {
    if((this_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        throw std::runtime_error("error create socket!");
    }
    struct sockaddr_in s_addr;
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(current_params.sport);
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(this_socket, (struct sockaddr *)&s_addr, sizeof(s_addr)) < 0) {
        close(this_socket);
        throw std::runtime_error("error bind socket with address!");
    }

    d_addr.sin_family = AF_INET;
    inet_pton(AF_INET, current_params.dest_ip.c_str(), &(d_addr.sin_addr));
    d_addr.sin_port = htons(current_params.dport);
}

UDPGenerator::~UDPGenerator() {
    close(this_socket);
}

void UDPGenerator::start() {
    std::thread sender([&] () {
        sigset_t smask;
        sigaddset(&smask, SIGINT);
        pthread_sigmask(SIG_BLOCK, &smask, nullptr);

        char msg[UDP_LENGTH_DATAGRAM];
        auto size_dgram = fillTestMessage(msg);
        while(t_running) {
            if (sendto(this_socket, msg, size_dgram, 0, (struct sockaddr*)&d_addr, sizeof(d_addr)) < 0) {
                throw std::runtime_error("error sendto function socket");
            }
        }
    });
    std::cout << "Begining sending..." << std::endl;
    std::cout << "For stopping send input \"ctrl+c\": " << std::endl;
    sender.join();
    std::cout << "Sending stopped" << std::endl;
}

size_t UDPGenerator::fillTestMessage(char * msg)
{
    size_t counter = 0;
    auto size_test_message = testMessage.length();
    auto c_str_test_message = testMessage.c_str();
    while(counter + size_test_message < UDP_LENGTH_DATAGRAM) {
        strcpy(msg + counter, c_str_test_message);
        counter += size_test_message;
    }
    msg[counter] = '\0';
    return counter;
}