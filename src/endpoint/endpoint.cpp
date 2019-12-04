#include "endpoint.hpp"

Endpoint::Endpoint(const sockaddr_in& addr) : _port(ntohs(addr.sin_port)) {
    auto ip_addr = std::make_unique<char[]>(INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &(addr.sin_addr), ip_addr.get(), INET_ADDRSTRLEN);
    _ipv4_addr = std::string(ip_addr.get());
}

sockaddr_in Endpoint::getAsSockAddr() const {
    sockaddr_in res{};
    res.sin_family = AF_INET;
    res.sin_port = htons(_port);
    inet_pton(AF_INET, _ipv4_addr.c_str(), &(res.sin_addr));
    return res;
}

std::string Endpoint::getIP() const {
    return _ipv4_addr;
}
uint16_t Endpoint::getPort() const {
    return _port;
}
void Endpoint::reset(const std::string& new_ipv4_addr, uint16_t new_port) {
    resetPort(new_port);
    resetIPv4Addr(new_ipv4_addr);
}
void Endpoint::reset(const sockaddr_in& addr) {
    char ip_addr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(addr.sin_addr), ip_addr, INET_ADDRSTRLEN);
    reset(std::string(ip_addr), ntohs(addr.sin_port));
}
void Endpoint::resetPort(uint16_t new_port) {
    _port = new_port;
}
void Endpoint::resetIPv4Addr(const std::string& new_addr) {
    sockaddr_in check_ip{};
    auto test = inet_pton(AF_INET, new_addr.c_str(), &(check_ip.sin_addr));
    if (test <= 0) {
        throw std::runtime_error("ERROR IP format string");
    }
    _ipv4_addr = new_addr;
}