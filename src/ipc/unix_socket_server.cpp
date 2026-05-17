#include "ipc/unix_socket_server.h"

#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

UnixSocketServer::UnixSocketServer(std::filesystem::path socket_path)
    : socket_path_(std::move(socket_path)) {}

UnixSocketServer::~UnixSocketServer() {
  Shutdown();
}

bool UnixSocketServer::Start() {
  server_fd_ = socket(AF_UNIX, SOCK_STREAM, 0);

  if (server_fd_ < 0) {
    return false;
  }

  sockaddr_un address{};
  address.sun_family = AF_UNIX;

  std::strncpy(address.sun_path, socket_path_.c_str(), sizeof(address.sun_path) - 1);

  unlink(socket_path_.c_str());

  if (bind(server_fd_, reinterpret_cast<sockaddr*>(&address), sizeof(address)) < 0) {
    return false;
  }

  if (listen(server_fd_, 5) < 0) {
    return false;
  }

  return true;
}

std::string UnixSocketServer::WaitMessage() {
  client_fd_ = accept(server_fd_, nullptr, nullptr);

  if (client_fd_ < 0) {
    return {};
  }

  char buffer[4096]{};
  ssize_t bytes = read(client_fd_, buffer, sizeof(buffer));

  if (bytes <= 0) {
    close(client_fd_);
    client_fd_ = -1;
    return {};
  }

  return std::string(buffer, bytes);
}

bool UnixSocketServer::SendResponse(const std::string& response) {
  if (client_fd_ < 0) {
    return false;
  }

  ssize_t bytes = write(client_fd_, response.c_str(), response.size());

  close(client_fd_);
  client_fd_ = -1;

  return bytes >= 0;
}

void UnixSocketServer::Shutdown() {
  if (client_fd_ != -1) {
    close(client_fd_);
    client_fd_ = -1;
  }

  if (server_fd_ != -1) {
    close(server_fd_);
    server_fd_ = -1;
  }

  unlink(socket_path_.c_str());
}
