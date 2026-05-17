#pragma once

#include <chrono>
#include <optional>
#include <string>

std::optional<std::chrono::system_clock::time_point> ParseDateTime(const std::string& datetime_str);

std::string FormatDateTime(std::chrono::system_clock::time_point tp);

std::chrono::seconds SecondsUntil(std::chrono::system_clock::time_point target) noexcept;

bool IsExpired(std::chrono::system_clock::time_point target) noexcept;
