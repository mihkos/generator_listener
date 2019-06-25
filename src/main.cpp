#include <algorithm>
#include <signal.h>
#include "clientconnectionmanager.hpp"
#include "serverconnectionmanager.hpp"
#include "clipp.h"
#include "nlohmann/json.hpp"

using namespace clipp;
volatile bool main_is_running = true;
Statistics statistics;


void signal_handler(int signum) {
    main_is_running = false;
}

void checkPorts(uint16_t& udp_port_X, uint16_t& udp_port_Y, uint16_t tcp_port)
{
    if(udp_port_X > udp_port_Y) {
        std::swap(udp_port_X, udp_port_Y);
    }
    if((udp_port_X <= tcp_port) & (tcp_port <= udp_port_Y))
        throw std::runtime_error("Bad tcp or udp ports values");
}
int main(int32_t argc, char** argv) {
    std::unique_ptr<ConnectionManager> connectionManager;
    try {
        enum class Mode{Client, Server, Help};
        json j_config;
        j_config["message"] = "test message";
        auto selected = Mode::Help;
        uint16_t tcp_port(0), backlog(5), number_udp_clients(0), number_tcp_clients(0), begin_client_port(0), udp_port_X(0), udp_port_Y(0);
        auto is_IP = [](const std::string& arg) {
            sockaddr_in check_ip;
            auto test = inet_pton(AF_INET, arg.c_str(), &(check_ip.sin_addr));
            return (test > 0); };
        auto clientMode = (
                command("client").set(selected,Mode::Client),
                (required("-tcp_clients") & integer("number", number_tcp_clients)) % "number_tcp_clients",
                (required("-tcp_dport") & integer("port", tcp_port)) % "destionation tcp port (on server)",
                (required("-dest_IP") & value(is_IP, "string_IP", j_config["dest_ip"])) % "destination IP (server IP)",
                (required("-sport") & integer("port", begin_client_port)) % "begin port for all clients (udp and tcp)",
                (required("-udp_clients") & integer("number", number_udp_clients)) % "number udp clients",
                (required("-udp_dports") & integer("b_port", udp_port_X) & integer("e_port", udp_port_Y)) % "begin and end destionation udp port (on server) (X-Y)",
                (option("-m --message") & value("test-message", j_config["message"])) % "test message (without space) for sending"
                );

        auto serverMode = (
                command("server").set(selected, Mode::Server),
                (required("-tcp_sport") & integer("port", tcp_port)) % "listening tcp port",
                (option("-b", "--backlog") & integer("size", backlog)) % "queue size pending connections for tcp",
                (required("-udp_sports") & integer("b_port", udp_port_X) & integer("e_port", udp_port_Y)) % "begin and end listening udp ports (X-Y)"
                );

        auto cli = (
                (clientMode | serverMode | command("help").set(selected,Mode::Help) ));

        if(parse(argc, argv, cli)) {
            switch(selected) {
                case Mode::Client:
                    checkPorts(udp_port_X, udp_port_Y, tcp_port);
                    j_config["udp_port_X"] = udp_port_X;
                    j_config["udp_port_Y"] = udp_port_Y;
                    j_config["tcp_port"] = tcp_port;
                    j_config["number_udp_clients"] = number_udp_clients;
                    j_config["number_tcp_clients"] = number_tcp_clients;
                    j_config["begin_sport"] = begin_client_port;
                    connectionManager = std::make_unique<ClientConnectionManager>(j_config);
                    break;
                case Mode::Server:
                    checkPorts(udp_port_X, udp_port_Y, tcp_port);
                    j_config["udp_port_X"] = udp_port_X;
                    j_config["udp_port_Y"] = udp_port_Y;
                    j_config["tcp_port"] = tcp_port;
                    j_config["backlog"] = backlog;
                    connectionManager = std::make_unique<ServerConnectionManager>(j_config);
                    break;
                case Mode::Help: std::cout << make_man_page(cli, "main");
                    main_is_running = false;
                    return 0;
            }
        } else {
            std::cout << usage_lines(cli, "main") << std::endl;
            throw std::runtime_error("error usage");
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
    }
    catch (const std::exception& error ) {
        main_is_running = false;
        if(connectionManager) {
            connectionManager->stop();
        }

        std::cerr << "Caught: " << error.what( ) << std::endl;
    };

    return 0;
}
