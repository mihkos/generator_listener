#pragma once

struct params {
    int8_t _type_app = -1; // flags -l or -g
    int8_t _type_proto = -1; // string "udp" or "tcp"
    uint16_t _sport = 0; // option -p
    uint16_t _dport = 0; //option -d
    std::string _dest_ip = "127.0.0.0"; //option -D
    std::string _test_message = "test message"; //option -m
};