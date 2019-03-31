#include "generator/generator.h"
#include "listener/listener.h"
#include "includes.h"
#define GENERATOR_TYPE 0
#define LISTENER_TYPE 1

struct params
{
    int type_app = -1; // flags -l or -g
    unsigned short sport = 0; // option -p
    unsigned short dport = 0; //option -d
    std::string d_ip = "127.0.0.0"; //option -D
    std::string test_message = "test message"; //option -m
};
void usage(char** argv)
{
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
int parseArgs(int argc, char** argv, struct params* current_params){
    int res;
    char* end;
    struct sockaddr_in check_ip;
    int test = 0;
    if(argc < 2)
    {
        usage(argv);
        return -1;
    }
    try {
        while ((res = getopt(argc, argv, "hlgp:D:d:m:")) != -1) {
            switch (res) {
                case 'h':
                    usage(argv);
                    return -1;
                case 'l':
                    current_params->type_app = LISTENER_TYPE;
                    break;
                case 'g':
                    current_params->type_app = GENERATOR_TYPE;
                    break;
                case 'p':
                    test = static_cast<int>(strtol(optarg, &end, 10));
                    if (optarg == end)
                        throw "Error value destionation port";
                    current_params->sport = static_cast<unsigned short>(test);
                    break;
                case 'D':

                    test = inet_pton(AF_INET, optarg, &(check_ip.sin_addr));
                    if (test <= 0)
                        throw "Error ip string";
                    current_params->d_ip = std::string(optarg);
                    break;
                case 'd':
                    test = static_cast<int>(strtol(optarg, &end, 10));
                    if (optarg == end)
                        throw "Error value port";
                    current_params->dport = static_cast<unsigned short>(test);
                    break;
                case 'm':
                    current_params->test_message = std::string(optarg);
                    break;
                case '?':
                    std::cout << "Error option!" << std::endl;
                    usage(argv);
                    return -1;
            };
        };
    }
    catch(const char * err)
    {
        std::cout << err << std::endl;
        return -1;
    }
    return 0;
};

int main(int argc, char** argv)
{
    struct params current_params;
    if(parseArgs(argc, argv, &current_params) < 0)
        return 0;
    if(current_params.type_app == GENERATOR_TYPE)
    {
        auto generator = new UDPGenerator();
        generator->testMessage = current_params.test_message;
        generator->startGenerator(current_params.sport, current_params.d_ip, current_params.dport);

    }
    if(current_params.type_app == LISTENER_TYPE)
    {
        auto listener = new UDPListener();
        listener->startListener(current_params.sport);
    }
    return 0;
}
