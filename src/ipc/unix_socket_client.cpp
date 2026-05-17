#include "ipc/unix_socket_client.h"

#include <cstring>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

UnixSocketClient::UnixSocketClient(std::filesystem::path socket_path)
    : socket_path_(std::move(socket_path)) {}

std::string UnixSocketClient::SendRequest(std::string_view request) {
  int socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);

  if (socket_fd < 0) {
    return {};
  }

  sockaddr_un address{};
  address.sun_family = AF_UNIX;

  std::strncpy(address.sun_path, socket_path_.c_str(), sizeof(address.sun_path) - 1);

  if (connect(socket_fd, reinterpret_cast<sockaddr*>(&address), sizeof(address)) < 0) {
    close(socket_fd);
    return {};
  }

  write(socket_fd, request.data(), request.size());

  char buffer[4096]{};

  ssize_t bytes = read(socket_fd, buffer, sizeof(buffer));

  close(socket_fd);

  if (bytes <= 0) {
    return {};
  }

  return std::string(buffer, bytes);
}
