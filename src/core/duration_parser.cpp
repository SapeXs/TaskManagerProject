#include "core/duration_parser.h"

#include <string>

namespace {

int64_t GetUnitMultiplier(std::string_view unit, bool& ok) {
  ok = true;

  if (unit.empty() || unit == "s" || unit == "sec" || unit == "second" || unit == "seconds") {
    return 1;
  }

  if (unit == "m" || unit == "min" || unit == "minute" || unit == "minutes") {
    return 60;
  }

  if (unit == "h" || unit == "hour" || unit == "hours") {
    return 60 * 60;
  }

  if (unit == "d" || unit == "day" || unit == "days") {
    return 24 * 60 * 60;
  }

  if (unit == "w" || unit == "week" || unit == "weeks") {
    return 7 * 24 * 60 * 60;
  }

  if (unit == "mo" || unit == "month" || unit == "months") {
    return 30 * 24 * 60 * 60;
  }

  if (unit == "y" || unit == "year" || unit == "years") {
    return 365 * 24 * 60 * 60;
  }

  ok = false;
  return 1;
}

bool AddWithOverflowCheck(int64_t lhs, int64_t rhs, int64_t& result) {
  if (rhs > 0 && lhs > INT64_MAX - rhs) {
    return false;
  }

  result = lhs + rhs;
  return true;
}

bool MulWithOverflowCheck(int64_t lhs, int64_t rhs, int64_t& result) {
  if (lhs != 0 && rhs > INT64_MAX / lhs) {
    return false;
  }

  result = lhs * rhs;
  return true;
}

}  // namespace

bool TryParseDuration(std::string_view value, int64_t& seconds, std::string& error) {
  seconds = 0;

  if (value.empty()) {
    error = "Duration is empty";
    return false;
  }

  std::size_t pos = 0;
  bool parsed_any_part = false;

  while (pos < value.size()) {
    while (pos < value.size() && std::isspace(static_cast<unsigned char>(value[pos]))) {
      ++pos;
    }

    if (pos >= value.size()) {
      break;
    }

    if (!std::isdigit(static_cast<unsigned char>(value[pos]))) {
      error = "Duration part must start with a number";
      return false;
    }

    std::size_t number_begin = pos;

    while (pos < value.size() && std::isdigit(static_cast<unsigned char>(value[pos]))) {
      ++pos;
    }

    std::string number_part(value.substr(number_begin, pos - number_begin));

    int64_t number = 0;

    try {
      number = std::stoll(number_part);
    } catch (...) {
      error = "Invalid duration number";
      return false;
    }

    while (pos < value.size() && std::isspace(static_cast<unsigned char>(value[pos]))) {
      ++pos;
    }

    std::size_t unit_begin = pos;

    while (pos < value.size() && std::isalpha(static_cast<unsigned char>(value[pos]))) {
      ++pos;
    }

    std::string_view unit = value.substr(unit_begin, pos - unit_begin);

    bool unit_ok = false;
    int64_t multiplier = GetUnitMultiplier(unit, unit_ok);

    if (!unit_ok) {
      error = "Unknown duration unit. Use s, m, h, d, w, mo, y";
      return false;
    }

    int64_t part_seconds = 0;

    if (!MulWithOverflowCheck(number, multiplier, part_seconds)) {
      error = "Duration is too large";
      return false;
    }

    int64_t new_total = 0;

    if (!AddWithOverflowCheck(seconds, part_seconds, new_total)) {
      error = "Duration is too large";
      return false;
    }

    seconds = new_total;
    parsed_any_part = true;
  }

  if (!parsed_any_part) {
    error = "Duration is empty";
    return false;
  }

  return true;
}
