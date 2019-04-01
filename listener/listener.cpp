
#include "listener.h"

UDPListener::UDPListener() = default;

UDPListener::~UDPListener() = default;

void UDPListener::startListener(unsigned short listen_port) {
    struct sockaddr_in addr;

    try {
        if ((this_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
            throw "socket function";
        addr.sin_family = AF_INET;
        addr.sin_port = htons(listen_port);
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        if (bind(this_socket, (struct sockaddr *) &addr, sizeof(addr)) < 0)
            throw "bind function";
        std::atomic<long> received_bytes(0);
        std::thread sleeper(&UDPListener::calculatingSpeed, this, &received_bytes);
        std::thread connection(&UDPListener::handle, this, &received_bytes);
        sleeper.join();
        connection.join();
    }
    catch(const char * err)
    {
        perror(err);
        closeListener();
    }
}
void UDPListener::calculatingSpeed(std::atomic<long>* received_bytes) {
    std::chrono::time_point<std::chrono::system_clock> start, end;
    long current_received_bytes;
    double res_speed;
    double mbt = 1024 * 1024;
    double elapsed_seconds;
    while(true) {
        start = std::chrono::system_clock::now();
        sleep(1);
        current_received_bytes = *received_bytes;
        *received_bytes = 0;
        end = std::chrono::system_clock::now();
        elapsed_seconds = std::chrono::duration_cast<std::chrono::microseconds>(end-start).count() / 1000000.0;
        res_speed = current_received_bytes / mbt / elapsed_seconds;
        std::cout << "speed: " <<  std::setw(10) << res_speed << " Mbps\r";
        std::cout.flush();
    }
}

void UDPListener::handle(std::atomic<long>* received_bytes) {
    char buf[UDP_LENGTH_DATAGRAMM];
    int bytes_read;
    while(true) {
        bytes_read = static_cast<int>(recvfrom(this_socket, buf, 32768, 0, NULL, NULL));
        buf[bytes_read] = '\0';
        *received_bytes += bytes_read;
        //std::cout << buf << std::endl;
    }
}

void UDPListener::closeListener() {
    close(this_socket);
}