#include "generator/generator.h"
#include "listener/listener.h"

#define GENERATOR_TYPE 0
#define LISTENER_TUPE 1

struct params
{
    int type_app = -1;
    unsigned short sport = 0;
    unsigned short dport = 0;
    std::string d_ip;
    std::string test_message = "test message";
};
int parseArgs(int argc, char** argv, struct params* current_params){
    int res;
    char* end;
    struct sockaddr_in check_ip;
    int test =0;
    while ( (res = getopt(argc,argv,"lgp:D:d:m:")) != -1){
        switch (res){
            case 'l':
                current_params->type_app = LISTENER_TUPE;
                break;
            case 'g':
                current_params->type_app = GENERATOR_TYPE;
                break;
            case 'p':
                current_params->sport = static_cast<unsigned short>(strtol(optarg, &end, 10));
                break;
            case 'D':
                test = inet_pton(AF_INET, optarg, &(check_ip.sin_addr));
                if(test)
                    current_params->d_ip = std::string(optarg);
                else
                    printf("Error ip!\n");
                break;
            case 'd':
                current_params->dport = static_cast<unsigned short>(strtol(optarg, &end, 10));
                break;
            case 'm':
                current_params->test_message = std::string(optarg);
                break;
            case '?':
                printf("Error option!\n");break;
        };
    };
    return 0;
};

int main(int argc, char** argv)
{
    struct params current_params;
    parseArgs(argc, argv, &current_params);
    if(current_params.type_app == GENERATOR_TYPE)
    {
        auto generator = new UDPGenerator();
        generator->testMessage = current_params.test_message;
        generator->startGenerator(current_params.sport, current_params.d_ip, current_params.dport);

    }
    else
    {
        auto listener = new UDPListener();
        listener->startListener(current_params.sport);
    }
    return 0;
}
