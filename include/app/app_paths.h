#pragma once

#include <filesystem>

namespace app_paths {

std::filesystem::path GetDataDirectory();
std::filesystem::path GetStoragePath();
std::filesystem::path GetSocketPath();

void EnsureDataDirectoryExists();

}  // namespace app_paths
