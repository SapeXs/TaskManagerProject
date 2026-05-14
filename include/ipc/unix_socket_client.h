#pragma once

#include <filesystem>
#include <string>

class UnixSocketClient {
 public:
  explicit UnixSocketClient(
      std::filesystem::path socket_path);

  bool SendMessage(
      const std::string& message);

 private:
  std::filesystem::path socket_path_;
};
