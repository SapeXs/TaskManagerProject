#pragma once

#include <vector>
#include <string>
#include "ui/command_type.h"

class Command {
public:
    Command(CommandType type, std::vector<std::string> args = {});

    CommandType GetType() const noexcept;
    const std::vector<std::string>& GetArgs() const noexcept;
    bool IsValid() const noexcept;

private:
    CommandType type_;
    std::vector<std::string> args_;
};