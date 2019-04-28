#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdexcept>
#include "../params.hpp"
#include <iostream>
#include <iomanip>
#include <cstring>
#include <climits>
#include <memory>

#define RECV_TIMEOUT_SECS 3
#define RECV_TIMEOUT_USECS 0

#define QUEUE_SIZE_PENDING_CONNECTIONS 5

extern volatile bool t_running;

class Socket {
public:
    Socket() = delete;
    Socket(const params& current_params);
    Socket(int32_t connected_socket, sockaddr_in client_addr) : _this_socket(connected_socket), _partner_addr(client_addr), _sport(0) {}
    Socket(Socket&& rhs) : _this_socket(rhs._this_socket), _sport(rhs._sport), _partner_addr(rhs._partner_addr) { rhs._this_socket = INT_MAX; }
    Socket& operator=(Socket&& rhs) {
        if (&rhs == this) {
            return *this;
        }
        _this_socket = rhs._this_socket;
        _sport = rhs._sport;
        _partner_addr = rhs._partner_addr;
        rhs._this_socket = INT_MAX;
        return *this;
    }
    ~Socket();
    Socket(const Socket& rhs) = default;
    Socket& operator=(const Socket& rhs) = default;
    void bind();
    Socket accept();
    void setPartner(const params& current_params);
    void setPartner(const sockaddr_in& addr);
    void resetSPort(const uint16_t& new_sport);
    uint16_t getSPort();
    void listen();
    void connect();
    std::string peername();
    uint32_t recv(char* buf, uint32_t max_recv_bytes);
    uint32_t recvfrom(char* buf, uint32_t max_recv_bytes, sockaddr_in* source_addr);
    void sendto(char* buf, uint32_t amount_send_bytes);
    void send(char* buf, uint32_t amount_send_bytes);
    int32_t get() {
        return _this_socket;
    }
private:
    int32_t _this_socket;
    uint16_t _sport;
    sockaddr_in _partner_addr;
};