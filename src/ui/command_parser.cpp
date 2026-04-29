#include "ui/command_parser.h"
#include <sstream>
#include <string>

std::vector<std::string> CommandParser::Split_(const std::string& input) const {
    std::vector<std::string> tokens;
    std::istringstream stream(input);
    std::string token;

    while (stream >> token) {
        tokens.push_back(std::move(token));
    }
    return tokens;
}

CommandType CommandParser::ParseCommandType_(const std::string& command_name) const {
    if (command_name == "add")      return CommandType::kAdd;
    if (command_name == "list")     return CommandType::kList;
    if (command_name == "find")     return CommandType::kFind;
    if (command_name == "remove")   return CommandType::kRemove;
    if (command_name == "filter")   return CommandType::kFilter;
    if (command_name == "help")     return CommandType::kHelp;
    if (command_name == "exit")     return CommandType::kExit;

    return CommandType::kInvalid;
}

Command CommandParser::Parse(const std::string& input) const {
    std::vector<std::string> tokens = Split_(input);

    if (tokens.empty()) {
        return Command(CommandType::kInvalid, {});
    }

    CommandType type = ParseCommandType_(tokens[0]);
    
    std::vector<std::string> args(tokens.begin() + 1, tokens.end());

    return Command(type, std::move(args));
}