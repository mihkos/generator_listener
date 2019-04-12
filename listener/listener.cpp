
#include "listener.hpp"

#define RECV_TIMEOUT_SECS 3
#define RECV_TIMEOUT_USECS 0

UDPListener::UDPListener(const struct params& current_params) {
    if ((this_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        throw std::runtime_error("socket function");
    }

    struct timeval tv;
    tv.tv_sec = RECV_TIMEOUT_SECS;
    tv.tv_usec = RECV_TIMEOUT_USECS;
    if(setsockopt(this_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        close(this_socket);
        throw std::runtime_error("setsockopt function");
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(current_params.sport);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(this_socket, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        close(this_socket);
        throw std::runtime_error("bind function");
    }
}

UDPListener::~UDPListener() {
    close(this_socket);
};

void UDPListener::start() {

    uint64_t received_bytes(0);
    std::thread sleeper([&] () {
        double one_mbt = 1024 * 128;
        uint64_t last_seen_received_bytes(0);
        auto start = std::chrono::system_clock::now();
        while(t_running) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            auto end = std::chrono::system_clock::now();
            double elapsed_seconds = std::chrono::duration<double>(end - start).count();
            double res_speed = (received_bytes - last_seen_received_bytes) / one_mbt / elapsed_seconds;
            last_seen_received_bytes = received_bytes;
            std::cout << "speed: " <<  std::fixed << std::setw(10) << std::setprecision(3) << res_speed << " Mbps\r";
            std::cout.flush();
            start = std::chrono::system_clock::now();
        }
    });
    std::thread receiver([&] () {
        char buf[UDP_LENGTH_DATAGRAM];
        while(t_running) {
            auto bytes_read = recvfrom(this_socket, buf, UDP_LENGTH_DATAGRAM, 0, nullptr, nullptr);
            if(errno == EAGAIN) {
                bytes_read = 0;
                errno = 0;
            }
            buf[bytes_read] = '\0';
            received_bytes += bytes_read;
        }
    });
    std::cout << "Listening..." << std::endl;
    std::cout << "For stopping listen input \"ctrl+c\": " << std::endl;
    sleeper.join();
    receiver.join();
    std::cout << std::endl << "Listening stopped" << std::endl;
}