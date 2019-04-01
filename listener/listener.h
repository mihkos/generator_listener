
#ifndef UDP_SERV_CLIENT_SERVER_H
#define UDP_SERV_CLIENT_SERVER_H

#include "../includes.h"
class UDPListener
{
public:
    UDPListener();
    ~UDPListener();
    void startListener(unsigned short listen_port);
    void closeListener();

private:
    int this_socket;
    void handle(std::atomic<long>* received_bytes);
    void calculatingSpeed(std::atomic<long>* received_bytes);
};


#endif //UDP_SERV_CLIENT_SERVER_H
