#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "env/env.hpp"
#include "utils/utils.hpp"

namespace CommandHandler {

enum class BuiltinType {
  TYPE = 0,
  ECHO_CMD,
  EXIT,
  PWD,
  CD,
  NOT_FOUND,
};

const std::unordered_map<std::string, BuiltinType> builtin_commands = {
    {"type", BuiltinType::TYPE}, {"echo", BuiltinType::ECHO_CMD},
    {"exit", BuiltinType::EXIT}, {"pwd", BuiltinType::PWD},
    {"cd", BuiltinType::CD},
};

BuiltinType getBuiltinType(const std::string &command);

typedef const utils::Command command_t;

class Completion {
private:
  std::vector<std::string> completions;
  std::string completions_of;

public:
  std::string getCompletion(std::string_view prefix, Env *env);

  void printLastCompletions() const noexcept;

  const std::string &getCompletionOf() const noexcept;

  size_t getCompletionSize() const noexcept;
};

int run(const std::string &exe_command, command_t &command, Env &env);

void handleType(const command_t &command, Env &env);

void changeDirectory(const std::string &path);

void handleEchoCommand(command_t &command);
}; // namespace CommandHandler
