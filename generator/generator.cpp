

#include "generator.hpp"

UDPGenerator::UDPGenerator() : UDPGenerator(0){}

UDPGenerator::UDPGenerator(unsigned short sport) {
    struct sockaddr_in s_addr;
    t_running = true;

    if((this_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        throw std::runtime_error("error create socket!");

    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(sport);
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(this_socket, (struct sockaddr *)&s_addr, sizeof(s_addr)) < 0)
        throw std::runtime_error("error bind socket with address!");
}


UDPGenerator::~UDPGenerator() {
    close(this_socket);
}
void UDPGenerator::setDestAddr(std::string dest_ip, unsigned short dport) {
    d_addr.sin_family = AF_INET;
    inet_pton(AF_INET, dest_ip.c_str(), &(d_addr.sin_addr));
    d_addr.sin_port = htons(dport);
}

void UDPGenerator::startGenerator() {
    boost::thread sender(&UDPGenerator::handle, this);
    std::cout << "Begining sending..." << std::endl;
    std::cout << "For stopping send input \"stop\": " << std::endl;
    sender.detach();

    std::string signal;
    do {
        std::cin >> signal;
    }
    while(signal != "stop");
    t_running = false;

    sender.interrupt();
    sender.join();
}
int UDPGenerator::fillTestMessage(char * msg)
{
    int counter = 0;
    int size_test_message = static_cast<int>(testMessage.length());
    const char * c_str_test_message = testMessage.c_str();
    while(counter + size_test_message < UDP_LENGTH_DATAGRAMM) {
        strcpy(msg + counter, c_str_test_message);
        counter += size_test_message;
    }
    msg[counter] = '\0';
    return counter;
}
void UDPGenerator::handle() {
    char msg[UDP_LENGTH_DATAGRAMM];
    auto size_dgram = static_cast<size_t >(fillTestMessage(msg));
    while(t_running) {
        if (sendto(this_socket, msg, size_dgram, 0, (struct sockaddr*)&d_addr, sizeof(d_addr)) < 0) {
            throw std::runtime_error("error sendto function socket");
        }
    }
}
