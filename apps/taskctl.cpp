#include <iostream>

#include "ipc/unix_socket_client.h"

int main(int argc, char** argv) {
  if (argc < 2) {
    return 1;
  }

  UnixSocketClient client("/tmp/taskmanager.sock");

  std::string response = client.SendRequest(argv[1]);

  std::cout << response << '\n';

  return 0;
}
