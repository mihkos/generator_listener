#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdexcept>

#define RECV_TIMEOUT_SECS 3
#define RECV_TIMEOUT_USECS 0

class Socket {
public:
    Socket() = delete;
    Socket(const params& current_params) {
        if(current_params._type_proto == IPPROTO_UDP) {
            if ((_this_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
                throw std::runtime_error("socket function");
            }
            timeval tv = {RECV_TIMEOUT_SECS, RECV_TIMEOUT_USECS};
            if(setsockopt(_this_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
                close(_this_socket);
                throw std::runtime_error("setsockopt function");
            }
        }
        else if(current_params._type_proto == IPPROTO_TCP) {
            if ((_this_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                throw std::runtime_error("socket function");
            }
            int32_t enable = 1;
            if (setsockopt(_this_socket, SOL_SOCKET, SO_REUSEADDR,  &enable, sizeof(enable)) < 0) {
                close(_this_socket);
                throw std::runtime_error("setsockopt function");
            }
        }

        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(current_params._sport);
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        if (bind(_this_socket, (sockaddr *) &addr, sizeof(addr)) < 0) {
            close(_this_socket);
            throw std::runtime_error("bind function");
        }
    }
    Socket(int32_t connected_socket) : _this_socket(connected_socket){}
    ~Socket() {
        shutdown(_this_socket, SHUT_WR);
        close(_this_socket);
    }
    auto get() {
        return _this_socket;
    }

private:
    int32_t _this_socket;
};