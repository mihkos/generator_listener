#include "connectionmanager.hpp"

void ConnectionManager::stop() {
    for (auto& thread : _threads) {
        *(thread._is_running.get()) = false;
    }
    for (auto& thread : _threads) {
        thread._thread.join();
    }
}

void ConnectionManager::collect_garbage() {
    auto end_iter = _threads.end();
    for(auto iter = _threads.begin(); iter != end_iter;) {
        if(!(*(iter->_is_running.get()))) {
            iter->_thread.join();
            std::lock_guard<std::mutex> lock_guard(_mutex);
            _threads.erase(iter++);
        }
        else { ++iter; }
    }
}

void ConnectionManager::run_connection(std::unique_ptr<Connection> connection) {
    auto is_running = std::make_shared<bool>(true);
    std::thread thread([this, connection(std::move(connection)), is_running] () mutable {
        try {
            do_run_connection(std::move(connection), is_running);
        }
        catch (const std::system_error& error) {
            if (error.code().value() == EINTR) {
                std::cout <<  "connection closed" << std::endl;
                *(is_running.get()) = false;
            } else {
                std::cout << "Caught: " << error.what() << std::endl;
                std::terminate();
            }
        }
    });

    std::lock_guard<std::mutex> lock_guard(_mutex);
    _threads.emplace_back(connection_thread{._thread = std::move(thread), ._is_running = std::move(is_running)});
}