#include "socket.hpp"

Socket::~Socket() {
    if(_nativeHandle >= 0) {
        close(_nativeHandle);
    }
}
void Socket::shutdown() {
    ::shutdown(_nativeHandle, SHUT_WR);
}
void Socket::setsockopt(int level, int optname, const void *optval, socklen_t optlen) {
    if (::setsockopt(_nativeHandle, level, optname,  optval, optlen) < 0) {
        throw std::system_error(std::error_code(errno, std::system_category()), "error setsockopt function");
    }
}
void Socket::bind(uint16_t sport) {
    sockaddr_in native_addr;
    native_addr.sin_family = AF_INET;
    native_addr.sin_port = htons(sport);
    native_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (::bind(_nativeHandle, (sockaddr *)&native_addr, sizeof(native_addr)) < 0) {
        throw std::system_error(std::error_code(errno, std::system_category()), "error bind function");
    }
}

Socket Socket::accept(Endpoint* endpoint) {
    sockaddr_in client_addr{};
    socklen_t client_addr_len = sizeof(client_addr);
    int32_t connection;
    connection = ::accept(_nativeHandle, (sockaddr *) &client_addr, &client_addr_len);

    if (connection < 0) {
        throw std::system_error(std::error_code(errno, std::system_category()), "error tcp accept function");
    }
    endpoint = new Endpoint(client_addr);
    return Socket(connection);
}

void Socket::listen(int32_t backlog) {
    if(::listen(_nativeHandle, backlog) < 0) {
        throw std::system_error(std::error_code(errno, std::system_category()), "error tcp listen function");
    }
}

void Socket::connect(const Endpoint& dest_addr) {
    auto partner_addr(dest_addr.getAsSockAddr());
    if (::connect(_nativeHandle, (struct sockaddr *)&partner_addr, sizeof(partner_addr)) < 0) {
        throw std::system_error(std::error_code(errno, std::system_category()), "error tcp connect function");
    }
}

std::string Socket::getpeername(const Endpoint& endpoint) {
    std::stringstream peer_stream;
    peer_stream  << endpoint.getIP() << ":" << endpoint.getPort();
    return peer_stream.str();
}

size_t Socket::recv(uint8_t* buf, size_t len, int32_t flags)
{
    int32_t bytes_recv(-1);
    if((bytes_recv = ::recv(_nativeHandle, buf, len, flags)) < 0) {
        throw std::system_error(std::error_code(errno, std::system_category()), "error recv function");
        //throw std::runtime_error("error recv function");
    }
    if (bytes_recv == 0) {
        throw std::system_error(std::error_code(EINTR, std::system_category()), "connection closed"); //std::runtime_error("connection closed");
    }
    return (size_t )bytes_recv;
}
size_t Socket::send(const uint8_t* buf, size_t len, int32_t flags) {
    int32_t bytes_sent(0);
    if((bytes_sent = ::send(_nativeHandle, buf, len, flags)) < 0) {
        throw std::system_error(std::error_code(errno, std::system_category()), "cannot send message");
    }
    return (size_t)bytes_sent;
}
size_t Socket::recvfrom(uint8_t *buf, size_t len, Endpoint* src_addr, int32_t flags) {
    sockaddr_in source_addr{};
    socklen_t source_addr_len = sizeof(source_addr);
    int32_t bytes_read(-1);
    if((bytes_read = ::recvfrom(_nativeHandle, buf, len, 0, (sockaddr *)&source_addr, &source_addr_len)) < 0) {
        throw std::system_error(std::error_code(errno, std::system_category()), "error recvfrom function");
    }
    src_addr->reset(source_addr);

    return (size_t)bytes_read;
}
size_t Socket::sendto(const uint8_t* buf, size_t len, const Endpoint& dest_addr, int32_t flags) {
    auto partner_addr(dest_addr.getAsSockAddr());
    int32_t bytes_sent(0);
    if ((bytes_sent = ::sendto(_nativeHandle, buf, len, 0, (sockaddr*)&partner_addr, sizeof(partner_addr))) < 0) {
        throw std::system_error(std::error_code(errno, std::system_category()), "error sendto function socket");
    }
    return (size_t)bytes_sent;
}

int32_t Socket::nativeHandle() const {
    return _nativeHandle;
}