#include "core/date_time_utils.h"

#include <algorithm>
#include <ctime>
#include <iomanip>
#include <sstream>

std::optional<std::chrono::system_clock::time_point> ParseDateTime(const std::string& datetime_str) {
  std::tm tm = {};
  tm.tm_isdst = -1;
  std::istringstream ss(datetime_str);

  if (datetime_str.find(':') != std::string::npos) {
    if (std::count(datetime_str.begin(), datetime_str.end(), ':') == 2) {
      ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    } else {
      ss >> std::get_time(&tm, "%Y-%m-%d %H:%M");
    }
  } else {
    ss >> std::get_time(&tm, "%Y-%m-%d");
    tm.tm_hour = 23;
    tm.tm_min = 59;
    tm.tm_sec = 59;
  }

  if (ss.fail()) {
    return std::nullopt;
  }

  std::time_t time = std::mktime(&tm);
  if (time == -1) {
    return std::nullopt;
  }

  return std::chrono::system_clock::from_time_t(time);
}

std::string FormatDateTime(std::chrono::system_clock::time_point tp) {
  std::time_t time = std::chrono::system_clock::to_time_t(tp);
  std::tm* tm = std::localtime(&time);

  std::ostringstream ss;
  ss << std::put_time(tm, "%Y-%m-%d %H:%M");
  return ss.str();
}

std::chrono::seconds SecondsUntil(std::chrono::system_clock::time_point target) noexcept {
  auto now = std::chrono::system_clock::now();
  return std::chrono::duration_cast<std::chrono::seconds>(target - now);
}

bool IsExpired(std::chrono::system_clock::time_point target) noexcept {
  return std::chrono::system_clock::now() >= target;
}
