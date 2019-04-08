
#pragma once

#define UDP_LENGTH_DATAGRAMM 32768

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <cstdlib>
#include <string>
#include <string.h>
#include <arpa/inet.h>
#include <iostream>
#include <iomanip>
#include <atomic>
#include <thread>
#include <csignal>
#include <signal.h>


namespace globalarea {
    extern volatile std::sig_atomic_t t_running;

}
