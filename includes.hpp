
#pragma once

#define UDP_LENGTH_DATAGRAM 16384

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
#include <arpa/inet.h>
#include <iostream>
#include <iomanip>
#include <atomic>
#include <thread>
#include <csignal>
#include <signal.h>

extern volatile bool t_running;

struct params {
    int8_t type_app = -1; // flags -l or -g
    uint16_t sport = 0; // option -p
    uint16_t dport = 0; //option -d
    std::string dest_ip = "127.0.0.0"; //option -D
    std::string test_message = "test message"; //option -m
};
