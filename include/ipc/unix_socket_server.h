#pragma once

#include <cstdint>
#include <filesystem>
#include <string>

class UnixSocketServer {
public:
    explicit UnixSocketServer(std::filesystem::path socket_path);

    ~UnixSocketServer();

    bool Start();

    std::string WaitMessage();
private:
    std::filesystem::path socket_path_;

    int32_t server_fd_ = -1;
    int32_t client_fd_ = -1;
};
