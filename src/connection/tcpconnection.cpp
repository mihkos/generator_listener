#include "tcpconnection.hpp"


TCPConnection::TCPConnection(uint16_t sport) : Connection() {
    _socket = std::make_unique<Socket>(AF_INET, SOCK_STREAM, 0);
    int32_t enable = 1;
    _socket->setsockopt(SOL_SOCKET, (SO_REUSEPORT | SO_REUSEADDR),  &enable, sizeof(enable));
    _socket->bind(sport);
    timeval tv = {Socket::Recv_Timeout_Secs, Socket::Recv_Timeout_Usecs};
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
void TCPConnection::doClientConnection(const uint8_t *outdata, size_t outsize, uint8_t *indata, size_t insize) {
    auto bytes_sent = send(outdata, outsize);
    statistics._bytes_sent += bytes_sent;
    auto bytes_read = receive(indata, insize);
    statistics._bytes_received += bytes_read;
}
void TCPConnection::doServerConnection(uint8_t *inOutData, size_t insize) {
    auto bytes_read = receive(inOutData, insize);
    statistics._bytes_received += bytes_read;
    auto bytes_sent = send(inOutData, bytes_read);
    statistics._bytes_sent += bytes_sent;
}