
#include "listener.h"

UDPListener::UDPListener() = default;

UDPListener::~UDPListener() = default;

void UDPListener::startListener(unsigned short listen_port ) {
    struct sockaddr_in addr;

    try {
        if ((this_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
            throw "socket function";
        addr.sin_family = AF_INET;
        addr.sin_port = htons(listen_port);
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        if (bind(this_socket, (struct sockaddr *) &addr, sizeof(addr)) < 0)
            throw "bind function";
        handle();
    }
    catch(const char * err)
    {
        perror(err);
        closeListener();
    }
}

void UDPListener::handle() {
    char buf[UDP_LENGTH_DATAGRAMM];
    int bytes_read;
    while(true)
    {
        bytes_read = static_cast<int>(recvfrom(this_socket, buf, 32768, 0, NULL, NULL));
        buf[bytes_read] = '\0';
        printf("%s\n", buf);
    }
}

void UDPListener::closeListener() {
    close(this_socket);
}