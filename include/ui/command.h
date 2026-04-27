#pragma once

#include <vector>
#include <string>

class Command {
public:
    CommandType GetType() const noexcept;
    const std::vector<std::string>& GetArgs() const noexcept;
    bool IsValid() const noexcept;

private:
    CommandType type_;
    std::vector<std::string> args_;
};