#include "udpconnection.hpp"

UDPConnection::UDPConnection(uint16_t sport, const Endpoint& partner_addr) : _partner_addr(partner_addr) {
        _socket = std::make_unique<Socket>(AF_INET, SOCK_DGRAM, 0);
        _socket->bind(sport);
        timeval tv = {Socket::Recv_Timeout_Secs, Socket::Recv_Timeout_Usecs};
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

void UDPConnection::startReceiver() {
    _receiver = std::thread([this] () {
        uint8_t buf[Lenght_Test_Message];
        while(main_is_running) {
            try {
                auto bytes_read = receive(buf, Lenght_Test_Message);
                statistics._bytes_received += bytes_read;
            }
            catch (const std::system_error& error) {
                if(error.code().value() != EAGAIN) {
                    throw error;
                }
            }
        }
    });
}

void UDPConnection::doClientConnection(const uint8_t *outdata, size_t outsize, uint8_t *indata, size_t insize) {
    auto bytes_sent = send(outdata, outsize);
    statistics._bytes_sent += bytes_sent;
}

void UDPConnection::doServerConnection(uint8_t *inOutData, size_t insize) {
    auto bytes_read = receive(inOutData, insize);
    statistics._bytes_received += bytes_read;
    auto bytes_sent = send(inOutData, bytes_read);
    statistics._bytes_sent += bytes_sent;
}