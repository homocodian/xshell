#pragma once

#include <string>
#include <vector>

#include "env.h"
#include "utils.h"

namespace CommandHandler {

const std::vector<std::string> builtin_commands = {"type", "echo", "exit",
                                                   "pwd", "cd"};

typedef const utils::Command command_t;

class Completion {
 private:
  std::vector<std::string> completions;
  std::string completions_of;

 public:
  std::string getCompletion(std::string_view prefix, Env* env);

  void printLastCompletions() const noexcept;

  const std::string& getCompletionOf() const noexcept;

  size_t getCompletionSize() const noexcept;
};

int run(const std::string& exe_command, command_t& command, Env& env);

void handleType(const command_t& command, Env& env);

void changeDirectory(const std::string& path);

void handleEchoCommand(command_t& command);
};  // namespace CommandHandler
