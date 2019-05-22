#pragma once

#include "socket.hpp"
#include "../statistics.hpp"
extern Statistics statistics;
extern volatile bool main_is_running;

#include <thread>
#include <mutex>
#include <list>
#include <cerrno>
#include <string>

struct Connection {
    Connection(const json& j_config) : _socket(nullptr){}
    Connection() = default;
    virtual ~Connection() = default;
    Connection(Connection&& other) = default;
    Connection& operator=(Connection&& other) {
        if (&other == this)
            return *this;
        _socket = std::move(other._socket);
        return *this;
    }
    virtual uint16_t send(const uint8_t* data, size_t size) = 0;
    virtual uint16_t receive(uint8_t* data, size_t size) = 0;

protected:
    std::unique_ptr<Socket> _socket;
};

