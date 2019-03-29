

#include "generator.h"

UDPGenerator::UDPGenerator() {}

UDPGenerator::~UDPGenerator() {}

void UDPGenerator::startGenerator(char * s_ip, unsigned short sport, char * d_ip, unsigned short dport ) {
    struct sockaddr_in s_addr;

    this_socket = socket(AF_INET, SOCK_DGRAM, 0);
    //errno
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(sport);
    inet_pton(AF_INET, s_ip, &(s_addr.sin_addr));
    //source ip

    bind(this_socket, (struct sockaddr *)&s_addr, sizeof(s_addr));
    //errno

    struct sockaddr_in d_addr;
    d_addr.sin_family = AF_INET;
    inet_pton(AF_INET, d_ip, &(d_addr.sin_addr));
    d_addr.sin_port = htons(dport);
    handle(d_addr);
}
void UDPGenerator::fillTestMessage(char * msg)
{
    int counter = 0;
    int size_test_message = static_cast<int>(testMessage.length());
    const char * c_str_test_message = testMessage.c_str();
    while(counter + size_test_message < UDP_LENGTH_DATAGRAMM)
    {
        strcpy(msg, c_str_test_message);
        msg += size_test_message;
        counter+=size_test_message;
    }
}
void UDPGenerator::handle(struct sockaddr_in d_addr) {
    char msg[UDP_LENGTH_DATAGRAMM];
    fillTestMessage(msg);
    while(1) {
        if (sendto(this_socket, msg, UDP_LENGTH_DATAGRAMM, MSG_NOSIGNAL, (struct sockaddr*)&d_addr, sizeof(d_addr)) < 0) {
            perror("sendto");
            close(this_socket);
        }
        sleep(1);
    }
}
void UDPGenerator::closeGenerator() {
    close(this_socket);
}