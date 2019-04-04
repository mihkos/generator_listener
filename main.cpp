#include "generator.hpp"
#include "listener.hpp"
#include "includes.hpp"

#define LISTENER_TYPE 0x1
#define GENERATOR_TYPE 0x2
#define OPTION_SPORT 0x4
#define OPTION_DEST_IP 0x8
#define OPTION_DPORT 0x10

#define RES_OPTION_LISTENER 0x5
#define RES_OPTION_GENERATOR 0x1E

struct params {
    int type_app = -1; // flags -l or -g
    unsigned short sport = 0; // option -p
    unsigned short dport = 0; //option -d
    std::string d_ip = "127.0.0.0"; //option -D
    std::string test_message = "test message"; //option -m
};

void usage(char** argv) {
    std::cout << "Use for udp_listener: " << argv[0] << "-l -p <listening_port>" << std::endl \
              << "or use for udp_generator: " << argv[0] << "-g -p <source_port> -D <dest_ip> "
                                                            "-d <dest_port> [-m <test_msg>]" << std::endl << std::endl;

    std::cout << "Help information: " \
    << "-h - help information" << std::endl \
    << "-l, -g - flags type of application (listener or generator)" << std::endl \
    << "-p <port> - (with -l or -g) option listening port or source port" << std::endl \
    << "only for -g flag: " << std::endl \
    << "\t -D <ip> - option destination ip" << std::endl \
    << "\t -d <port> - option destination port" << std::endl \
    << "\t -m <string_without_space> - option for set test message without space(for now)" << std::endl;
}

int parseArgs(int argc, char** argv, struct params* current_params) {
    int res;
    char* end;
    struct sockaddr_in check_ip;
    int test = 0;
    unsigned char res_option = 0;
    if(argc < 2) {
        usage(argv);
        return -1;
    }
    while ((res = getopt(argc, argv, "hlgp:D:d:m:")) != -1) {
        switch (res) {
            case 'h':
                usage(argv);
                return -1;
            case 'l':
                current_params->type_app = LISTENER_TYPE;
                res_option += LISTENER_TYPE;
                break;
            case 'g':
                current_params->type_app = GENERATOR_TYPE;
                res_option += GENERATOR_TYPE;
                break;
            case 'p':
                test = static_cast<int>(strtol(optarg, &end, 10));
                if (optarg == end)
                    throw std::runtime_error("Error value destionation port");
                current_params->sport = static_cast<unsigned short>(test);
                res_option += OPTION_SPORT;
                break;
            case 'D':
                test = inet_pton(AF_INET, optarg, &(check_ip.sin_addr));
                if (test <= 0)
                    throw std::runtime_error("Error ip string");
                current_params->d_ip = std::string(optarg);
                res_option += OPTION_DEST_IP;
                break;
            case 'd':
                test = static_cast<int>(strtol(optarg, &end, 10));
                if (optarg == end)
                    throw std::runtime_error("Error value port");
                current_params->dport = static_cast<unsigned short>(test);
                res_option += OPTION_DPORT;
                break;
            case 'm':
                current_params->test_message = std::string(optarg);
                break;
            case '?':
                std::cerr << "Error option!" << std::endl;
                usage(argv);
                return -1;
        };
    };
    if(res_option != RES_OPTION_GENERATOR && res_option != RES_OPTION_LISTENER) {
        std::cerr << "Error options!" << std::endl;
        usage(argv);
        return -1;
    }

    return 0;
};


int main(int argc, char** argv) {
    try
    {
        struct params current_params;
        if(parseArgs(argc, argv, &current_params) < 0)
            return 0;

        if(current_params.type_app == GENERATOR_TYPE) {
            UDPGenerator generator(current_params.sport);
            generator.testMessage = current_params.test_message;
            generator.setDestAddr(current_params.d_ip, current_params.dport);
            generator.startGenerator();

        }
        if(current_params.type_app == LISTENER_TYPE) {
            auto listener = new UDPListener(current_params.sport);
            listener->startListener();

        }
    }
    catch ( const std::exception& error )
    {
        std::cerr << "Caught: " << error.what( ) << std::endl;
    };

    return 0;
}
