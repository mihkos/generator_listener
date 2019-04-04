
#include "listener.hpp"


UDPListener::UDPListener(unsigned short listen_port) {
    struct sockaddr_in addr;
    t_running = true;

    if ((this_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        throw "socket function";
    addr.sin_family = AF_INET;
    addr.sin_port = htons(listen_port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(this_socket, (struct sockaddr *) &addr, sizeof(addr)) < 0)
        throw std::runtime_error("bind function");
}


UDPListener::~UDPListener() {
    close(this_socket);
};

void UDPListener::startListener() {

    std::atomic<long> received_bytes(0);
    boost::thread sleeper(&UDPListener::calculatingSpeed, this, &received_bytes);
    boost::thread receiver(&UDPListener::handle, this, &received_bytes);
    std::cout << "Listening..." << std::endl;
    std::cout << "For stopping send input \"stop\": " << std::endl;
    sleeper.detach();
    receiver.detach();

    std::string signal;
    do {
        std::cin >> signal;
    }
    while(signal != "stop");
    t_running = false;

    sleeper.interrupt();
    receiver.interrupt();

    sleeper.join();
    receiver.join();
}
void UDPListener::calculatingSpeed(std::atomic<long>* received_bytes) {
    boost::chrono::time_point<boost::chrono::system_clock> start, end;
    long current_received_bytes;
    double res_speed;
    double mbt = 1024 * 1024;
    double elapsed_seconds;
    while(t_running) {
        start = boost::chrono::system_clock::now();
        boost::this_thread::sleep_for(boost::chrono::seconds(1));
        current_received_bytes = *received_bytes;
        *received_bytes = 0;
        end = boost::chrono::system_clock::now();
        elapsed_seconds = boost::chrono::duration_cast<boost::chrono::microseconds>(end-start).count() / 1000000.0;
        res_speed = current_received_bytes / mbt / elapsed_seconds;
        std::cout << "speed: " <<  std::setw(10) << res_speed << " Mbps\r";
        std::cout.flush();
    }
}

void UDPListener::handle(std::atomic<long>* received_bytes) {
    char buf[UDP_LENGTH_DATAGRAMM];
    int bytes_read;
    while(t_running) {
        bytes_read = static_cast<int>(recvfrom(this_socket, buf, 32768, 0, nullptr, nullptr));
        buf[bytes_read] = '\0';
        *received_bytes += bytes_read;
    }
}
