#include "ipc/unix_socket_server.h"

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

UnixSocketServer::UnixSocketServer(
    std::filesystem::path socket_path)
    : socket_path_(std::move(socket_path)) {}

UnixSocketServer::~UnixSocketServer() {
  if (client_fd_ != -1) {
    close(client_fd_);
  }

  if (server_fd_ != -1) {
    close(server_fd_);
  }

  unlink(socket_path_.c_str());
}

bool UnixSocketServer::Start() {
  server_fd_ = socket(AF_UNIX, SOCK_STREAM, 0);

  if (server_fd_ < 0) {
    return false;
  }

  sockaddr_un address{};
  address.sun_family = AF_UNIX;

  std::strncpy(
      address.sun_path,
      socket_path_.c_str(),
      sizeof(address.sun_path) - 1);

  unlink(socket_path_.c_str());

  if (bind(server_fd_,
           reinterpret_cast<sockaddr*>(&address),
           sizeof(address)) < 0) {
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

  char buffer[1024]{};

  ssize_t bytes =
      read(client_fd_, buffer, sizeof(buffer));

  if (bytes <= 0) {
    return {};
  }

  return std::string(buffer, bytes);
}
