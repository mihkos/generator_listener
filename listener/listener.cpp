
#include "listener.hpp"


UDPListener::UDPListener(unsigned short listen_port) {
    if ((this_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        throw "socket function";
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(listen_port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(this_socket, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        close(this_socket);
        throw std::runtime_error("bind function");
    }
}

UDPListener::~UDPListener() {
    close(this_socket);
};

void UDPListener::startListener() {

    std::atomic<long> received_bytes(0);
    std::thread sleeper([&] {calculatingSpeed(&received_bytes);});
    std::thread receiver([&] {handle(&received_bytes);});
    std::cout << "Listening..." << std::endl;
    std::cout << "For stopping listen input \"ctrl+c\": " << std::endl;
    sleeper.join();
    receiver.join();
    std::cout << "Listening stopped" << std::endl;
}
void UDPListener::calculatingSpeed(std::atomic<long>* received_bytes) {
    sigset_t smask;
    sigaddset(&smask, SIGINT);
    pthread_sigmask(SIG_BLOCK, &smask, nullptr);
    double mbt = 1024 * 1024;

    while(globalarea::t_running) {
        std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        long current_received_bytes = *received_bytes;
        *received_bytes = 0;
        std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
        double elapsed_seconds = std::chrono::duration<double>(end - start).count();
        double res_speed = current_received_bytes / mbt / elapsed_seconds;
        std::cout << "speed: " <<  std::fixed << std::setw(10) << res_speed << " Mbps\r";
        std::cout.flush();
    }
}

void UDPListener::handle(std::atomic<long>* received_bytes) {
    sigset_t smask;
    sigaddset(&smask, SIGINT);
    pthread_sigmask(SIG_BLOCK, &smask, nullptr);
    char buf[UDP_LENGTH_DATAGRAMM];
    while(globalarea::t_running) {
        int bytes_read = recvfrom(this_socket, buf, 32768, 0, nullptr, nullptr);
        buf[bytes_read] = '\0';
        *received_bytes += bytes_read;
    }
}
