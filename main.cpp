#include "generator.hpp"
#include "listener.hpp"
#include "includes.hpp"

#define LISTENER_TYPE 0x1
#define GENERATOR_TYPE 0x2
#define OPTION_SPORT 0x4
#define OPTION_DEST_IP 0x8
#define OPTION_DPORT 0x10
#define OPTION_HELP 0x20

#define RES_OPTION_LISTENER 0x5
#define RES_OPTION_GENERATOR 0x1E
#define RES_OPTION_HELP 0x20


volatile bool t_running = true;

std::string usage(char** argv) {
    std::stringstream help_stream;
    help_stream << "Use for udp_listener: " << argv[0] << " -l -p <listening_port>" << std::endl \
              << "or use for udp_generator: " << argv[0] << " -g -p <source_port> -D <dest_ip> "
                                                            "-d <dest_port> [-m <test_msg>]" << std::endl << std::endl;

    help_stream << "Help information: " \
    << "-h - help information" << std::endl \
    << "-l, -g - flags type of application (listener or generator)" << std::endl \
    << "-p <port> - (with -l or -g) option listening port or source port" << std::endl \
    << "only for -g flag: " << std::endl \
    << "\t -D <ip> - option destination ip" << std::endl \
    << "\t -d <port> - option destination port" << std::endl \
    << "\t -m <string_without_space> - option for set test message without space(for now)" << std::endl;
    return help_stream.str();
}

int parseArgs(int argc, char** argv, struct params* current_params) {
    int32_t res;
    uint8_t res_option = 0;
    if(argc < 2) {
        throw std::runtime_error(std::string("Not enough parameters!\n") + usage(argv));
    }
    while ((res = getopt(argc, argv, "hlgp:D:d:m:")) != -1) {
        switch (res) {
            case 'h': {
                res_option += OPTION_HELP;
                break;
            }
            case 'l': {
                current_params->type_app = LISTENER_TYPE;
                res_option += LISTENER_TYPE;
                break;
            }
            case 'g': {
                current_params->type_app = GENERATOR_TYPE;
                res_option += GENERATOR_TYPE;
                break;
            }
            case 'p': {
                char *end;
                auto test = strtol(optarg, &end, 10);
                if (optarg == end) {
                    throw std::runtime_error("ERROR value destionation port");
                }
                current_params->sport = static_cast<unsigned short>(test);
                res_option += OPTION_SPORT;
                break;
            }
            case 'D': {
                struct sockaddr_in check_ip;
                auto test = inet_pton(AF_INET, optarg, &(check_ip.sin_addr));
                if (test <= 0) {
                    throw std::runtime_error("ERROR IP format string");
                }
                current_params->dest_ip = std::string(optarg);
                res_option += OPTION_DEST_IP;
                break;
            }
            case 'd': {
                char *end;
                auto test = strtol(optarg, &end, 10);
                if (optarg == end) {
                    throw std::runtime_error("ERROR value port");
                }
                current_params->dport = static_cast<unsigned short>(test);
                res_option += OPTION_DPORT;
                break;
            }
            case 'm': {
                current_params->test_message = std::string(optarg);
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
    else if(res_option != RES_OPTION_GENERATOR && res_option != RES_OPTION_LISTENER ) {
        throw std::runtime_error("ERROR options set! For help use -h");
    }
};

void signal_handler(int signum)
{
    t_running = false;
}
int main(int argc, char** argv) {
    try
    {
        struct params current_params;
        parseArgs(argc, argv, &current_params);

        struct sigaction sa;
        sigset_t newset;
        sigemptyset(&newset);
        sigaddset(&newset, SIGINT);
        sa.sa_handler = signal_handler;
        sigaction(SIGINT, &sa, 0);

        if(current_params.type_app == GENERATOR_TYPE) {
            UDPGenerator generator(current_params);
            generator.testMessage = current_params.test_message;
            generator.start();
        }

        if(current_params.type_app == LISTENER_TYPE) {
            UDPListener listener(current_params);
            listener.start();
        }
    }
    catch ( const std::exception& error ) {
        std::cerr << "Caught: " << error.what( ) << std::endl;
        return 1;
    };

    return 0;
}
