#include "commands/command_parser.h"

#include <sstream>
#include <string>
#include <unordered_map>

namespace {
const std::unordered_map<std::string, CommandType> kCommandMap = {
    {"add", CommandType::kAdd},
    {"list", CommandType::kList},
    {"find", CommandType::kFind},
    {"remove", CommandType::kRemove},
    {"filter", CommandType::kFilter},
    {"help", CommandType::kHelp},
    {"exit", CommandType::kExit},
    {"save", CommandType::kSave},
    {"clear", CommandType::kClear},

    {"set-title", CommandType::kSetTitle},
    {"set-description", CommandType::kSetDescription},
    {"set-priority", CommandType::kSetPriority},
    {"add-tag", CommandType::kAddTag},
    {"remove-tag", CommandType::kRemoveTag},
    {"set-time", CommandType::kSetTime},
    {"set-interval", CommandType::kSetInterval},
    {"set-repeats", CommandType::kSetRepeats},
    {"add-value", CommandType::kAddValue},
    {"advance", CommandType::kAdvance},
    {"overdue", CommandType::kOverdue},
    {"reset", CommandType::kReset},

    {"set-deadline", CommandType::kSetDeadline},
    {"set-date", CommandType::kSetDate}};
}  // namespace

std::vector<std::string> CommandParser::Split_(const std::string& input) const {
  std::vector<std::string> tokens;
  tokens.reserve(10);

  std::string current_token;
  bool in_quotes = false;
  bool escaped = false;

  for (char ch : input) {
    if (escaped) {
      current_token += ch;
      escaped = false;
      continue;
    }

    if (ch == '\\') {
      escaped = true;
      continue;
    }

    if (ch == '"') {
      in_quotes = !in_quotes;
      continue;
    }

    if ((ch == ' ' || ch == '\t') && !in_quotes) {
      if (!current_token.empty()) {
        tokens.push_back(std::move(current_token));
        current_token.clear();
      }

      continue;
    }

    current_token += ch;
  }

  if (escaped) {
    current_token += '\\';
  }

  if (!current_token.empty()) {
    tokens.push_back(std::move(current_token));
  }

  return tokens;
}

CommandType CommandParser::ParseCommandType_(const std::string& command_name) const {
  auto it = kCommandMap.find(command_name);
  return (it != kCommandMap.end()) ? it->second : CommandType::kInvalid;
}

Command CommandParser::Parse(const std::string& input) const {
  std::vector<std::string> tokens = Split_(input);

  if (tokens.empty()) {
    return Command(CommandType::kInvalid, {});
  }

  std::vector<std::string> args;
  if (tokens.size() > 1) {
    args.assign(std::next(tokens.begin()), tokens.end());
  }

  return Command(ParseCommandType_(tokens[0]), std::move(args));
}
