#pragma once

#include <filesystem>
#include <string>

class UnixSocketClient {
 public:
  explicit UnixSocketClient(std::filesystem::path socket_path);

  std::string SendRequest(const std::string& request);

 private:
  std::filesystem::path socket_path_;
};
