#include <iostream>

#include "ipc/unix_socket_client.h"

int main(int argc, char** argv) {
  if (argc < 2) {
    return 1;
  }

  UnixSocketClient client("/tmp/taskmanager.sock");

  if (!client.SendMessage(argv[1])) {
    std::cerr << "Failed to send message\n";
    return 1;
  }

  return 0;
}
