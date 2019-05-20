#include "udpconnection.hpp"

UDPConnection::UDPConnection(uint16_t sport, const Endpoint& partner_addr) : _partner_addr(partner_addr) {
        _socket = std::make_unique<Socket>(AF_INET, SOCK_DGRAM, 0);
        _socket->bind(sport);
        timeval tv = {RECV_TIMEOUT_SECS, RECV_TIMEOUT_USECS};
        _socket->setsockopt(SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
}

void UDPConnection::setPartner(const Endpoint& endpoint) {
    _partner_addr = endpoint;
}
uint16_t UDPConnection::send(const uint8_t* data, size_t size)  {
    if(_partner_addr.getIP().empty()) {
        throw std::runtime_error("no installed destination endpoint");
    }
    return (uint16_t) _socket->sendto(data, size, _partner_addr, 0);
}
uint16_t UDPConnection::receive(uint8_t* data, size_t size) {
    size_t recv_bytes(0);
    recv_bytes = _socket->recvfrom(data, size, &_partner_addr, 0);
    return (uint16_t)recv_bytes;
}