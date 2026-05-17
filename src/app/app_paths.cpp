#include "app/app_paths.h"

#include <cstdlib>
#include <filesystem>
#include <string>

namespace app_paths {
namespace {

std::filesystem::path GetHomeDirectory() {
  const char* home = std::getenv("HOME");

  if (home == nullptr || std::string(home).empty()) {
    return ".";
  }

  return home;
}

}  // namespace

std::filesystem::path GetDataDirectory() {
  const char* xdg_data_home = std::getenv("XDG_DATA_HOME");

  if (xdg_data_home != nullptr && !std::string(xdg_data_home).empty()) {
    return std::filesystem::path(xdg_data_home) / "taskmanager";
  }

  return GetHomeDirectory() / ".local" / "share" / "taskmanager";
}

std::filesystem::path GetStoragePath() {
  return GetDataDirectory() / "tasks.txt";
}

std::filesystem::path GetSocketPath() {
  const char* xdg_runtime_dir = std::getenv("XDG_RUNTIME_DIR");

  if (xdg_runtime_dir != nullptr && !std::string(xdg_runtime_dir).empty()) {
    return std::filesystem::path(xdg_runtime_dir) / "taskmanager.sock";
  }

  return "/tmp/taskmanager.sock";
}

void EnsureDataDirectoryExists() {
  std::filesystem::create_directories(GetDataDirectory());
}

}  // namespace app_paths
