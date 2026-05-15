#include <utility>

#include "commands/command.h"

Command::Command(CommandType type, std::vector<std::string> args)
    : type_(type), args_(std::move(args)) {}

CommandType Command::GetType() const noexcept {
  return type_;
}

std::span<const std::string> Command::GetArgs() const noexcept {
  return args_;
}

bool Command::IsValid() const noexcept {
  return type_ != CommandType::kInvalid;
}
