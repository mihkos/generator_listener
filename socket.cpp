#include "socket.hpp"

Socket::Socket(const params& current_params) : _sport(current_params._sport) {
    if(current_params._type_proto == IPPROTO_UDP) {
        if ((_this_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            throw std::runtime_error("socket function");
        }
        timeval tv = {RECV_TIMEOUT_SECS, RECV_TIMEOUT_USECS};
        if(setsockopt(_this_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
            close(_this_socket);
            throw std::runtime_error("setsockopt function for recv_timeout");
        }
    }
    else if(current_params._type_proto == IPPROTO_TCP) {
        if ((_this_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            throw std::runtime_error("socket function");
        }
        timeval tv = {RECV_TIMEOUT_SECS, RECV_TIMEOUT_USECS};
        if(setsockopt(_this_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
            close(_this_socket);
            throw std::runtime_error("setsockopt function for recv_timeout");
        }
        int32_t enable = 1;
        if (setsockopt(_this_socket, SOL_SOCKET, SO_REUSEADDR,  &enable, sizeof(enable)) < 0) {
            close(_this_socket);
            throw std::runtime_error("setsockopt function");
        }
    }
}

Socket::~Socket() {
    shutdown(_this_socket, SHUT_WR);
    close(_this_socket);
}

void Socket::bind() {
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_sport);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (::bind(_this_socket, (sockaddr *) &addr, sizeof(addr)) < 0) {
        close(_this_socket);
        throw std::runtime_error("bind function");
    }
}

Socket Socket::accept() {
    while(t_running) {
        fd_set set;
        FD_ZERO(&set);
        FD_SET(_this_socket, &set);
        timeval timeout = {0, 0};
        auto count = select(_this_socket + 1, &set, 0, 0, &timeout);
        if (count) {
            sockaddr_in client_addr;
            socklen_t client_addr_len = sizeof(client_addr);
            auto connection = ::accept(_this_socket, (sockaddr *) &client_addr, &client_addr_len);

            if (connection < 0) {
                throw std::runtime_error("tcp accept function");
            }

            return Socket(connection, client_addr);
        }
    }
}

void Socket::setPartner(const params& current_params) {
    _partner_addr.sin_family = AF_INET;
    inet_pton(AF_INET, current_params._dest_ip.c_str(), &(_partner_addr.sin_addr));
    _partner_addr.sin_port = htons(current_params._dport);
}

void Socket::setPartner(const sockaddr_in& addr) {
    _partner_addr = sockaddr_in(addr);
}

void Socket::listen() {
    if(::listen(_this_socket, QUEUE_SIZE_PENDING_CONNECTIONS) < 0) {
        throw std::runtime_error("tcp listen function");
    }
}

void Socket::connect() {
    if (::connect(_this_socket, (struct sockaddr *)&_partner_addr, sizeof(_partner_addr)) < 0) {
        throw std::runtime_error("tcp connect function");
    }
}

std::string Socket::peername() {
    std::stringstream peer_stream;
    auto ip_addr = std::make_unique<char[]>(INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &(_partner_addr.sin_addr), ip_addr.get(), INET_ADDRSTRLEN);
    peer_stream  << ip_addr.get() << ":" << ntohs(_partner_addr.sin_port);
    return peer_stream.str();
}

int32_t Socket::recv(char* buf, int max_recv_bytes) {
    int32_t bytes_read(0);
    do {
        errno = 0;
        bytes_read = ::recv(_this_socket, buf, max_recv_bytes, 0);
    } while ((errno == EAGAIN) && (t_running != 0));
    if (bytes_read == 0) {
        throw std::runtime_error("connection closed");
    }
    return bytes_read;
}

int32_t Socket::recvfrom(char* buf, int max_recv_bytes, sockaddr_in* source_addr) {
    socklen_t source_addr_len = sizeof(*source_addr);
    int32_t bytes_read(0);
    do {
        errno = 0;
        bytes_read = ::recvfrom(_this_socket, buf, max_recv_bytes, 0, (sockaddr *) source_addr, &source_addr_len);
    } while ((errno == EAGAIN) && (t_running != 0));
    return bytes_read;
}

void Socket::sendto(char* buf, int amount_send_bytes) {
    if (::sendto(_this_socket, buf, amount_send_bytes, 0, (sockaddr*)&_partner_addr, sizeof(_partner_addr)) < 0) {
        throw std::runtime_error("error sendto function socket");
    }
}

void Socket::send(char* buf, int amount_send_bytes) {
    if(::send(_this_socket, buf, amount_send_bytes, 0) < 0) {
        throw std::runtime_error("cannot send message");
    }
}