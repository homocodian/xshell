#ifndef COMMANDHDLR_H
#define COMMANDHDLR_H

#include <string>
#include <vector>

#include "env.h"
#include "utils.h"

namespace CommandHandler {

const std::vector<std::string> builtin_commands = {"type", "echo", "exit",
                                                   "pwd", "cd"};

typedef const utils::Command command_t;

std::optional<std::string_view> searchCommand(const std::string_view &prefix);

int run(const std::string &exe_command, command_t &command, Env &env);

void handleType(command_t &command, Env &env);

void changeDirectory(const std::string &path);

void handleEchoCommand(command_t &command);
}; // namespace CommandHandler

#endif
