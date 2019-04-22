#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdexcept>
#include "params.hpp"
#include <iostream>
#include <iomanip>

#define RECV_TIMEOUT_SECS 3
#define RECV_TIMEOUT_USECS 0

#define QUEUE_SIZE_PENDING_CONNECTIONS 5

extern volatile bool t_running;

class Socket {
public:
    Socket() = delete;
    Socket(const params& current_params);
    Socket(int32_t connected_socket, sockaddr_in client_addr) : _this_socket(connected_socket), _partner_addr(client_addr){}
    ~Socket();
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;
    void bind();
    Socket accept();
    void setPartner(const params& current_params);
    void setPartner(const sockaddr_in& addr);
    void listen();
    void connect();
    std::string peername();
    int32_t recv(char* buf, int max_recv_bytes);
    int32_t recvfrom(char* buf, int max_recv_bytes, sockaddr_in* source_addr);
    void sendto(char* buf, int amount_send_bytes);
    void send(char* buf, int amount_send_bytes);
private:
    int32_t _this_socket;
    uint16_t _sport;
    sockaddr_in _partner_addr;
};