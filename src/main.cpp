#include <algorithm>
#include <signal.h>
#include "clientconnectionmanager.hpp"
#include "serverconnectionmanager.hpp"

#include "nlohmann/json.hpp"

volatile bool main_is_running = true;
Statistics statistics;
json parseCmdl(int argc, char** argv)
{
    json j_config;
    if(argc != 2) {
        throw std::runtime_error("Not enough parameters!");
    }
    int32_t res;
    while ((res = getopt(argc, argv, "sc")) != -1) {
        switch (res) {
            case 'c': {
                j_config["mode"] = 0;
                break;
            }
            case 's': {
                j_config["mode"] = 1;
                break;
            }
            case '?': {
                throw std::runtime_error("ERROR options set! USE: -s for server or -c for client");
            }
        }
    }
    return j_config;
}
void signal_handler(int signum) {
    main_is_running = false;
}

int main(int argc, char** argv) {
    try {
        json j_config = parseCmdl(argc, argv);

        std::unique_ptr<ConnectionManager> connectionManager;

        switch (j_config["mode"].get<uint8_t>()) {
            case 0:  //Mode::Client:
                j_config["number_udp_clients"] = 0;
                j_config["number_tcp_clients"] = 1;
                j_config["dest_ip"] = "127.0.0.1";
                j_config["message"] = "test message";
                j_config["begin_sport"] = 5090;
                j_config["begin_dport"] = 5060;
                j_config["end_dport"] = 5070;
                j_config["tcp_dport"] = 5050;
                connectionManager = std::make_unique<ClientConnectionManager>(j_config);
                break;
            case 1: //Mode::Server:
                j_config["tcp_sport"] = 5050;
                j_config["backlog"] = 5;
                j_config["start_sport"] = 5060;
                j_config["end_sport"] = 5061;
                connectionManager = std::make_unique<ServerConnectionManager>(j_config);
                break;
            default:
                throw std::runtime_error("Error select mode app");
        }

        connectionManager->start();

        struct sigaction sa;
        sa.sa_handler = signal_handler;
        sigaction(SIGINT, &sa, nullptr);
        while (main_is_running) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            statistics.print();
            connectionManager->collect_garbage();
        }
        connectionManager->stop();
        std::cout << std::endl;
    }
    catch (const std::exception& error ) {
        std::cerr << "Caught: " << error.what( ) << std::endl;
        std::terminate();
    };

    return 0;
}
