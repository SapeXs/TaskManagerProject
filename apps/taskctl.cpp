#include <iostream>
#include <string>
#include <string_view>

#include "app/app_paths.h"
#include "ipc/unix_socket_client.h"

namespace {

bool NeedsQuotes(std::string_view value) {
  return value.find(' ') != std::string_view::npos || value.find('\t') != std::string_view::npos ||
         value.find('"') != std::string_view::npos || value.find('\\') != std::string_view::npos;
}

std::string QuoteArg(std::string_view value) {
  if (!NeedsQuotes(value)) {
    return std::string(value);
  }

  std::string result = "\"";

  for (char ch : value) {
    if (ch == '"' || ch == '\\') {
      result += '\\';
    }

    result += ch;
  }

  result += '"';
  return result;
}

}  // namespace

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

    request += QuoteArg(argv[i]);
  }

  UnixSocketClient client(app_paths::GetSocketPath());
  std::string response = client.SendRequest(request);

  if (response.empty()) {
    std::cerr << "No response from daemon\n";
    return 1;
  }

  std::cout << response << '\n';
  return 0;
}
