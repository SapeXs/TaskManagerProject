#include <chrono>
#include <csignal>
#include <iostream>

#include "app/daemon_app.h"

DaemonApp* g_app = nullptr;

void HandleSignal(int signal) {
  if (g_app != nullptr) {
    std::cout << "\nReceived signal: " << signal << '\n';

    g_app->Stop();
  }
}

int main() {
  DaemonApp app(std::chrono::seconds(10), "/tmp/taskmanager.sock");

  g_app = &app;

  std::signal(SIGINT, HandleSignal);
  std::signal(SIGTERM, HandleSignal);

  app.Run();

  g_app = nullptr;

  return 0;
}
