
#include "listener.hpp"
#define QUEUE_SIZE_PENDING_CONNECTIONS 5

UDPListener::UDPListener(const params& current_params) : Listener(current_params){

}

UDPListener::~UDPListener() = default;

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
        auto buf = std::make_unique<char[]>(UDP_LENGTH_DATAGRAM);
        int32_t bytes_read(0);
        sockaddr_in client_addr;
        socklen_t client_addr_len(sizeof(client_addr));
        while(t_running) {
            do {
                errno = 0;
                bytes_read = recvfrom(_this_socket, buf.get(), UDP_LENGTH_DATAGRAM, 0, (sockaddr *)&client_addr, &client_addr_len);
            } while ((errno == EAGAIN) && (t_running != 0));
            buf[bytes_read] = '\0';
            received_bytes += bytes_read;

            if(t_running) {
                if (sendto(_this_socket, buf.get(), (size_t)bytes_read, 0, (sockaddr *) &client_addr, sizeof(client_addr)) < 0) {
                    throw std::runtime_error("error sendto function socket");
                }
            }
        }
    });
    std::cout << "Listening..." << std::endl;
    std::cout << "For stopping listen input \"ctrl+c\": " << std::endl;
    sleeper.join();
    receiver.join();
    std::cout << std::endl << "Listening stopped" << std::endl;
}

TCPListener::TCPListener(const params& current_params) : Listener(current_params) {

}

void TCPListener::start() {

    if(listen(_this_socket, QUEUE_SIZE_PENDING_CONNECTIONS) < 0) {
        throw std::runtime_error("tcp listen function");
    }

    std::atomic<uint64_t> received_bytes(0);
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

        auto ip_addr = std::make_unique<char[]>(INET_ADDRSTRLEN);
        sockaddr_in client_addr;
        socklen_t client_addr_len(sizeof(client_addr));
        fd_set set;
        while(t_running) {
            FD_ZERO(&set);
            FD_SET(_this_socket, &set);
            timeval timeout = {0, 0};
            auto count = select(_this_socket + 1, &set, 0, 0, &timeout);
            if (count) {
                int32_t connected_socket = accept(_this_socket, (sockaddr *) &client_addr, &client_addr_len);
                if (connected_socket < 0) {
                    throw std::runtime_error("tcp accept function");
                }
                inet_ntop(AF_INET, &(client_addr.sin_addr), ip_addr.get(), INET_ADDRSTRLEN);
                std::cout << std::endl << "Connected client: " << ip_addr.get() << ":"
                          << ntohs(client_addr.sin_port) << std::endl;
                _connetions.emplace_back(std::thread([&, connected_socket]() {
                    auto buf = std::make_unique<char[]>(TCP_LENGTH_MSG);
                    int32_t bytes_read(0);
                    while (t_running) {
                        bytes_read = recv(connected_socket, buf.get(), TCP_LENGTH_MSG, 0);
                        if (bytes_read == 0) {
                            std::cout << "Close connect" << std::endl;
                            break;
                        }
                        buf[bytes_read] = '\0';
                        received_bytes += bytes_read;

                        if (t_running) {
                            if (send(connected_socket, buf.get(), (size_t) bytes_read, 0) < 0) {
                                break;
                            }
                        }
                    }
                    shutdown(connected_socket, SHUT_WR);
                    close(connected_socket);
                }));
            }
        }
        for (auto& connetion : _connetions) {
            if(connetion.joinable()) {
                connetion.join();
            }
        }
    });
    std::cout << "Listening..." << std::endl;
    std::cout << "For stopping listen input \"ctrl+c\": " << std::endl;
    sleeper.join();
    receiver.join();
    std::cout << std::endl << "Listening stopped" << std::endl;
}

TCPListener::~TCPListener() = default;