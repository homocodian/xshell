#ifndef COMMANDHDLR_H
#define COMMANDHDLR_H

#include <string>
#include <vector>

#include "env.h"
#include "utils.h"

namespace CommandHandler {

typedef const utils::Command command_t;

int run(const std::string &exe_command, command_t &command, Env &env);

void handleType(command_t &command,
                const std::vector<const char *> &builtin_commands, Env &env);

void changeDirectory(const std::string &path);

void handleEchoCommand(command_t &command);
}; // namespace CommandHandler

#endif
