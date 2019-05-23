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

int main(int argc, char** argv) {
    try {

        json j_config;
        auto is_IP = [](const std::string& arg) {
            sockaddr_in check_ip;
            auto test = inet_pton(AF_INET, arg.c_str(), &(check_ip.sin_addr));
            return (test > 0); };
        enum class Mode{Client, Server, Help};
        Mode selected = Mode::Help;
        uint16_t tcp_sport(0), backlog(0), start_sport(0), end_sport(0), number_udp_clients(0), number_tcp_clients(0),
        begin_sport(0), begin_dport(0), end_dport(0), tcp_dport(0);
        auto clientMode = (
                command("client").set(selected,Mode::Client),
                (required("-tcp_clients") & integer("number", number_tcp_clients)) % "number_tcp_clients",
                (required("-tcp_dport") & integer("port", tcp_dport)) % "destionation tcp port (on server)",
                (required("-dest_IP") & value(is_IP, "string_IP", j_config["dest_ip"])) % "destination IP (server IP)",
                (required("-sport") & integer("port", begin_sport)) % "begin port for all clients (udp and tcp)",
                (required("-udp_clients") & integer("number", number_udp_clients)) % "number udp clients",
                (required("-udp_dports") & integer("b_port", begin_dport) & integer("e_port", end_dport)) % "begin and end destionation udp port (on server) (X Y)"
                );

        auto serverMode = (
                command("server").set(selected, Mode::Server),
                (required("-tcp_sport") & integer("port", tcp_sport)) % "listening tcp port",
                (required("-b", "--backlog") & integer("size", backlog)) % "queue size pending connections for tcp",
                (required("-udp_sports") & integer("b_port", start_sport) & integer("e_port", end_sport)) % "begin and end listening udp ports (X Y)"
                );

        auto cli = (
                (clientMode | serverMode | command("help").set(selected,Mode::Help) ));
        std::unique_ptr<ConnectionManager> connectionManager;
        if(parse(argc, argv, cli)) {
            switch(selected) {
                case Mode::Client:
                    j_config["number_udp_clients"] = number_udp_clients;
                    j_config["number_tcp_clients"] = number_tcp_clients;
                    j_config["message"] = "test message";
                    j_config["begin_sport"] = begin_sport;
                    j_config["begin_dport"] = begin_dport;
                    j_config["end_dport"] = end_dport;
                    j_config["tcp_dport"] = tcp_dport;
                    connectionManager = std::make_unique<ClientConnectionManager>(j_config);
                    break;
                case Mode::Server:
                    j_config["tcp_sport"] = tcp_sport;
                    j_config["backlog"] = backlog;
                    j_config["start_sport"] = start_sport;
                    j_config["end_sport"] = end_sport;
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
        std::cerr << "Caught: " << error.what( ) << std::endl;
    };

    return 0;
}
