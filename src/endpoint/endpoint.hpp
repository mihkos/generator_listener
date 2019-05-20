#pragma once

#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <memory>

struct Endpoint {

    Endpoint() = delete;
    Endpoint(std::string ipv4_addr, uint16_t port) : _ipv4_addr(std::move(ipv4_addr)), _port(port) {}
    Endpoint(const sockaddr_in& addr);
    Endpoint(const Endpoint& other) = default;
    Endpoint& operator=(const Endpoint& other) {
        _ipv4_addr = other._ipv4_addr;
        _port = other._port;
    }
    ~Endpoint() = default;

    sockaddr_in getAsSockAddr() const ;
    std::string getIP() const;
    uint16_t getPort() const;
    void reset(const std::string& new_ipv4_addr, uint16_t new_port);
    void reset(const sockaddr_in& addr);
    void resetPort(uint16_t new_port);
    void resetIPv4Addr(const std::string& new_addr);

private:
    uint16_t _port;
    std::string _ipv4_addr;
};