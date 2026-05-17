#pragma once

#include <filesystem>
#include <string>
#include <string_view>

class UnixSocketClient {
 public:
  explicit UnixSocketClient(std::filesystem::path socket_path);

  std::string SendRequest(std::string_view request);

 private:
  std::filesystem::path socket_path_;
};
