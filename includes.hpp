
#pragma once

#define UDP_LENGTH_DATAGRAM 16384
#define TCP_LENGTH_MSG 1024
#define RECV_TIMEOUT_SECS 3
#define RECV_TIMEOUT_USECS 0

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <string>
#include <string.h>
#include <algorithm>
#include <arpa/inet.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <thread>
#include <csignal>
#include <signal.h>
#include <atomic>

extern volatile bool t_running;

struct params {
    int8_t type_app = -1; // flags -l or -g
    int8_t type_proto = -1; // string "udp" or "tcp"
    uint16_t sport = 0; // option -p
    uint16_t dport = 0; //option -d
    std::string dest_ip = "127.0.0.0"; //option -D
    std::string test_message = "test message"; //option -m
};

class Listener {
public:
    Listener() = default;
    Listener(const params& current_params) {
        if(current_params.type_proto == IPPROTO_TCP) {
            if ((_this_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                throw std::runtime_error("socket function");
            }
            int32_t enable = 1;
            if (setsockopt(_this_socket, SOL_SOCKET, SO_REUSEADDR,  &enable, sizeof(enable)) < 0) {
                close(_this_socket);
                throw std::runtime_error("setsockopt function");
            }
        }
        else if(current_params.type_proto == IPPROTO_UDP) {
            if ((_this_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
                throw std::runtime_error("socket function");
            }
            timeval tv = {RECV_TIMEOUT_SECS, RECV_TIMEOUT_USECS};
            if(setsockopt(_this_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
                close(_this_socket);
                throw std::runtime_error("setsockopt function");
            }
        }
        else {
            throw std::runtime_error("socket function");
        }

        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(current_params.sport);
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        if (bind(_this_socket, (sockaddr *) &addr, sizeof(addr)) < 0) {
            close(_this_socket);
            throw std::runtime_error("bind function");
        }
    }
    virtual void start() = 0;
    virtual ~Listener(){
        close(_this_socket);
    }
protected:
    int32_t _this_socket;
};

class Generator {
public:
    Generator() = default;
    Generator(const params& current_params) {
        if(current_params.type_proto == IPPROTO_TCP) {
            if((_this_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                throw std::runtime_error("error create socket!");
            }
            int32_t enable = 1;
            if (setsockopt(_this_socket, SOL_SOCKET, SO_REUSEADDR,  &enable, sizeof(enable)) < 0) {
                close(_this_socket);
                throw std::runtime_error("setsockopt function");
            }
        }
        else if(current_params.type_proto == IPPROTO_UDP) {
            if((_this_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
                throw std::runtime_error("error create socket!");
            }
            timeval timeout = {RECV_TIMEOUT_SECS, RECV_TIMEOUT_USECS};
            if(setsockopt(_this_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
                close(_this_socket);
                throw std::runtime_error("setsockopt function");
            }
        }

        sockaddr_in s_addr;
        s_addr.sin_family = AF_INET;
        s_addr.sin_port = htons(current_params.sport);
        s_addr.sin_addr.s_addr = htonl(INADDR_ANY);

        if(bind(_this_socket, (sockaddr *)&s_addr, sizeof(s_addr)) < 0) {
            close(_this_socket);
            throw std::runtime_error("error bind socket with address!");
        }

        _d_addr.sin_family = AF_INET;
        inet_pton(AF_INET, current_params.dest_ip.c_str(), &(_d_addr.sin_addr));
        _d_addr.sin_port = htons(current_params.dport);
    }
    virtual void start() = 0;
    virtual ~Generator(){
        close(_this_socket);
    }
protected:
    int32_t _this_socket;
    sockaddr_in _d_addr;
    std::vector<char> _testMessage;
    static std::vector<char> createTestMessage(std::string templ_msg, size_t length_msg)
    {
        std::vector<char> res_test_msg;
        size_t counter = 0;
        auto size_test_message = templ_msg.length();
        while(counter + size_test_message < length_msg) {
            std::copy(templ_msg.begin(), templ_msg.end(), std::back_inserter(res_test_msg));
            counter += size_test_message;
        }
        std::copy(templ_msg.c_str(), templ_msg.c_str()+(length_msg - counter), std::back_inserter(res_test_msg));
        return res_test_msg;
    }
};