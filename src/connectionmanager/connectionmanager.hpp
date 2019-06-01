#pragma once

#include "connection.hpp"


struct connection_thread {
    std::thread _thread;
    std::shared_ptr<bool> _is_running;
};
struct ConnectionManager {
    virtual void start() = 0;
    virtual void stop();
    void collect_garbage();
    void run_connection(std::unique_ptr<Connection> connection);

protected:
    std::mutex _mutex;
    std::list<connection_thread> _threads;

    virtual void do_run_connection(std::unique_ptr<Connection> connection, std::shared_ptr<bool> is_running) = 0;
};