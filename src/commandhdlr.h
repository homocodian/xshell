#ifndef COMMANDHDLR_H
#define COMMANDHDLR_H

#include <set>
#include <string>
#include <vector>

#include "env.h"

namespace CommandHandler {

int run(const std::string &command, const std::vector<std::string> &args);

void handleType(const std::vector<std::string> &tokens,
                const char *const builtin_commands[],
                size_t builtin_commands_size, Env &env);
}; // namespace CommandHandler

#endif
