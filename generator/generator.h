

#ifndef UDP_GENERATOR_LISTENER_GENERATOR_H
#define UDP_GENERATOR_LISTENER_GENERATOR_H

#include "../includes.h"

class UDPGenerator{
public:
    UDPGenerator();
    ~UDPGenerator();
    void startGenerator(char * s_ip, unsigned short sport, char * d_ip, unsigned short dport);
    void closeGenerator();

    std::string testMessage;
private:
    int this_socket;
    void handle(struct sockaddr_in daddr);
    void fillTestMessage(char * msg);
};
#endif //UDP_GENERATOR_LISTENER_GENERATOR_H
