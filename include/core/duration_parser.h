#pragma once

#include <cstdint>
#include <string>
#include <string_view>

bool TryParseDuration(std::string_view value, int64_t& seconds, std::string& error);
