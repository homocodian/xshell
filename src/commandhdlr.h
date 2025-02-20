#ifndef COMMANDHDLR_H
#define COMMANDHDLR_H

#include <string>
#include <vector>

#include "env.h"

namespace CommandHandler {

int run(const std::string &command, const std::vector<std::string> &args,
        Env &env);

void handleType(const std::vector<std::string> &tokens,
                const std::vector<const char *> &builtin_commands, Env &env);

void changeDirectory(const std::string &path);
}; // namespace CommandHandler

#endif
