#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdexcept>
#include "nlohmann/json.hpp"
#include "../endpoint/endpoint.hpp"
#include <system_error>

#include <iomanip>
#include <cstring>
#include <climits>
#include <memory>

#define RECV_TIMEOUT_SECS 2
#define RECV_TIMEOUT_USECS 0

using json = nlohmann::json;
struct Socket {
public:
    Socket() = delete;
    Socket(int32_t domain, int32_t type, int32_t protocol) {
        if ((_nativeHandle = socket(domain, type, protocol)) < 0) {
            throw std::runtime_error("socket function");
        }
    };
    Socket(const int32_t& connected_socket): _nativeHandle(connected_socket) { }
    Socket(Socket&& rhs) {
        std::swap(_nativeHandle, rhs._nativeHandle);
        rhs._nativeHandle = -1;
    }
    Socket& operator=(Socket&& rhs) {
        if (&rhs == this) {
            return *this;
        }
        std::swap(_nativeHandle, rhs._nativeHandle);
        rhs._nativeHandle = -1;
        return *this;
    }
    ~Socket();
    Socket(const Socket& rhs) = delete;
    Socket& operator=(const Socket& rhs) = delete;
    void setsockopt(int level, int optname, const void *optval, socklen_t optlen);
    void bind(uint16_t sport);
    Socket accept(Endpoint* endpoint);
    void listen(int32_t backlog);
    void connect(const Endpoint& dest_addr);
    std::string getpeername(const Endpoint& endpoint);

    size_t recv(uint8_t* buf, size_t len, int32_t flags = 0);
    size_t send(const uint8_t* buf, size_t len, int32_t flags = 0);
    size_t recvfrom(uint8_t *buf, size_t len, Endpoint* src_addr, int32_t flags = 0);
    size_t sendto(const uint8_t* buf, size_t len, const Endpoint& dest_addr, int32_t flags = 0);
    void shutdown();

    int32_t nativeHandle() const;
private:
    int32_t _nativeHandle = -1;
};


