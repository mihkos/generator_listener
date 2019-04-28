#include <algorithm>
#include <signal.h>

#include "tcplistener.hpp"
#include "udplistener.hpp"
#include "tcpgenerator.hpp"
#include "udpgenerator.hpp"

#define LISTENER_TYPE 0x1
#define GENERATOR_TYPE 0x2
#define OPTION_SPORT 0x4
#define OPTION_DEST_IP 0x8
#define OPTION_DPORT 0x10
#define OPTION_NUMBER_OF_SESSION 0x20
#define OPTION_HELP 0x40

#define RES_OPTION_LISTENER 0x5
#define RES_OPTION_GENERATOR 0x1E
#define RES_OPTION_MULTI_GENERATOR 0x3E
#define RES_OPTION_HELP OPTION_HELP

volatile bool t_running = true;

std::string usage(char** argv) {
    std::stringstream help_stream;
    help_stream << "Use for listener: " << argv[0] << " -l <udp|tcp> -p <listening_port>" << std::endl \
              << "or use for generator: " << argv[0] << " -g <udp|tcp> -p <source_port> -D <dest_ip> "
                                                        "-d <dest_port> [-n <sessions>] [-m <test_msg>]"
                                                        << std::endl << std::endl;
    help_stream << "Help information: " \
    << "-h - help information" << std::endl \
    << "-l <udp|tcp> - option type of application listener for proto UDP or TCP" << std::endl \
    << "-g <udp|tcp> - option type of application generator for proto UDP or TCP" << std::endl \
    << "-p <port> - (with -l or -g) option listening port or source port" << std::endl \
    << "only for -g flag: " << std::endl \
    << "\t -D <ip> - option destination ip" << std::endl \
    << "\t -d <port> - option destination port" << std::endl \
    << "\t -n <number_of_sessions> - option number of session (for tcp generator)" << std::endl \
    << "\t -m <string_without_space> - option for set test message without space(for now)" << std::endl;
    return help_stream.str();
}

params parseArgs(int argc, char** argv) {
    int32_t res;
    uint8_t res_option = 0;
    if(argc < 2) {
        throw std::runtime_error(std::string("Not enough parameters!\n") + usage(argv));
    }
    params current_params;
    while ((res = getopt(argc, argv, "hl:g:p:D:d:n:m:")) != -1) {
        switch (res) {
            case 'h': {
                res_option += OPTION_HELP;
                break;
            }
            case 'l': {
                current_params._type_app = LISTENER_TYPE;
                res_option += LISTENER_TYPE;
                std::string proto(optarg);
                std::transform(proto.begin(), proto.end(), proto.begin(), ::tolower);
                if(proto == "udp")
                    current_params._type_proto = IPPROTO_UDP;
                else if(proto == "tcp")
                    current_params._type_proto = IPPROTO_TCP;
                else
                    throw std::runtime_error("ERROR type protocol for app");
                break;
            }
            case 'g': {
                current_params._type_app = GENERATOR_TYPE;
                res_option += GENERATOR_TYPE;
                std::string proto(optarg);
                std::transform(proto.begin(), proto.end(), proto.begin(), ::tolower);
                if(proto == "udp")
                    current_params._type_proto = IPPROTO_UDP;
                else if(proto == "tcp")
                    current_params._type_proto = IPPROTO_TCP;
                else
                    throw std::runtime_error("ERROR type protocol for app");
                break;
            }
            case 'p': {
                char *end;
                auto test = strtol(optarg, &end, 10);
                if (optarg == end) {
                    throw std::runtime_error("ERROR value destionation port");
                }
                current_params._sport = static_cast<unsigned short>(test);
                res_option += OPTION_SPORT;
                break;
            }
            case 'D': {
                sockaddr_in check_ip;
                auto test = inet_pton(AF_INET, optarg, &(check_ip.sin_addr));
                if (test <= 0) {
                    throw std::runtime_error("ERROR IP format string");
                }
                current_params._dest_ip = std::string(optarg);
                res_option += OPTION_DEST_IP;
                break;
            }
            case 'd': {
                char *end;
                auto test = strtol(optarg, &end, 10);
                if (optarg == end) {
                    throw std::runtime_error("ERROR value port");
                }
                current_params._dport = static_cast<unsigned short>(test);
                res_option += OPTION_DPORT;
                break;
            }
            case 'n': {
                char *end;
                auto test = strtol(optarg, &end, 10);
                if (optarg == end) {
                    throw std::runtime_error("ERROR value number of session");
                }
                if(test < 0) {
                    throw std::runtime_error("ERROR value number of session");
                }
                current_params._number_sessions = (uint32_t)test;
                res_option += OPTION_NUMBER_OF_SESSION;
                break;
            }
            case 'm': {
                current_params._test_message = std::string(optarg);
                break;
            }
            case '?': {
                throw std::runtime_error("ERROR options set! For help use -h");
            }
        };
    };
    if(res_option == RES_OPTION_HELP) {
        std::cout << usage(argv);
    }
    else if(res_option != RES_OPTION_GENERATOR &&
            res_option != RES_OPTION_LISTENER &&
            res_option != RES_OPTION_MULTI_GENERATOR) {
        throw std::runtime_error("ERROR options set! For help use -h");
    }
    return current_params;
};

void signal_handler(int signum) {
    t_running = false;
}

int main(int argc, char** argv) {
    try
    {
        auto current_params = parseArgs(argc, argv);

        struct sigaction sa;
        sa.sa_handler = signal_handler;
        sigaction(SIGINT, &sa, 0);
        if(current_params._type_app == GENERATOR_TYPE)
        {
            std::shared_ptr<Generator> generator;
            if (current_params._type_proto == IPPROTO_UDP) {
                generator.reset(new UDPGenerator(current_params));
            }

            if(current_params._type_proto == IPPROTO_TCP) {
                generator.reset(new TCPGenerator(current_params));
            }
            generator->start();
        }
        if(current_params._type_app == LISTENER_TYPE)
        {
            std::shared_ptr<Listener> listener;
            if (current_params._type_proto == IPPROTO_UDP) {
                listener.reset(new UDPListener(current_params));
            }

            if(current_params._type_proto == IPPROTO_TCP) {
                listener.reset(new TCPListener(current_params));
            }
            listener->start();
        }
    }
    catch (const std::exception& error ) {
        std::cerr << "Caught: " << error.what( ) << std::endl;
        return 1;
    };

    return 0;
}
