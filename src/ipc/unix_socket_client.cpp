#include "ipc/unix_socket_client.h"

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <cstring>

UnixSocketClient::UnixSocketClient(
    std::filesystem::path socket_path)
    : socket_path_(std::move(socket_path)) {}

bool UnixSocketClient::SendMessage(
    const std::string& message) {
  int socket_fd =
      socket(AF_UNIX, SOCK_STREAM, 0);

  if (socket_fd < 0) {
    return false;
  }

  sockaddr_un address{};
  address.sun_family = AF_UNIX;

  std::strncpy(
      address.sun_path,
      socket_path_.c_str(),
      sizeof(address.sun_path) - 1);

  if (connect(socket_fd,
              reinterpret_cast<sockaddr*>(&address),
              sizeof(address)) < 0) {
    close(socket_fd);
    return false;
  }

  write(socket_fd,
        message.c_str(),
        message.size());

  close(socket_fd);

  return true;
}
