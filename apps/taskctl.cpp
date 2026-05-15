#include <iostream>
#include <string>

#include "ipc/unix_socket_client.h"

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cerr << "Usage: taskctl <command> [args...]\n";
    return 1;
  }

  std::string request;

  for (int i = 1; i < argc; ++i) {
    if (i > 1) {
      request += ' ';
    }

    request += argv[i];
  }

  UnixSocketClient client("/tmp/taskmanager.sock");
  std::string response = client.SendRequest(request);

  if (response.empty()) {
    std::cerr << "No response from daemon\n";
    return 1;
  }

  std::cout << response << '\n';
  return 0;
}
