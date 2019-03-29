
#include "listener.h"

UDPListener::UDPListener() {}

UDPListener::~UDPListener() {}

void UDPListener::startListener(unsigned short listen_port ) {
    struct sockaddr_in addr;
    this_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if(this_socket < 0)
    {
        perror("socket not started");
        exit(1);
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(listen_port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(this_socket, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind socket with IP and port");
        exit(2);
    }
    handle();
}

void UDPListener::handle() {
    char buf[UDP_LENGTH_DATAGRAMM];
    int bytes_read;
    while(true)
    {
        bytes_read = recvfrom(this_socket, buf, 32768, 0, NULL, NULL);
        buf[bytes_read] = '\0';
        printf("%s\n", buf);
    }
}

void UDPListener::closeListener() {
    close(this_socket);
}