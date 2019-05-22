#include "tcpconnection.hpp"


TCPConnection::TCPConnection(uint16_t sport) : Connection() {
    _socket = std::make_unique<Socket>(AF_INET, SOCK_STREAM, 0);
    int32_t enable = 1;
    _socket->setsockopt(SOL_SOCKET, (SO_REUSEPORT | SO_REUSEADDR),  &enable, sizeof(enable));
    _socket->bind(sport);
    timeval tv = {RECV_TIMEOUT_SECS, RECV_TIMEOUT_USECS};
    _socket->setsockopt(SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

}
TCPConnection::TCPConnection(std::unique_ptr<Socket> connected_socket) {
    _socket = std::move(connected_socket);
}
void TCPConnection::connect_client_to_server(const Endpoint& server_addr) {
    _socket->connect(server_addr);
}
uint16_t TCPConnection::send(const uint8_t* data, size_t size) {
    return (uint16_t )_socket->send(data, size, 0);
}
uint16_t TCPConnection::receive(uint8_t* data, size_t size) {
    return (uint16_t )_socket->recv(data, size, 0);
}