#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "commands/handler.hpp"
#include "debug/utils.hpp"
#include "env/env.hpp"
#include "inputs/handler.hpp"
#include "utils/utils.hpp"

int main() {
  Env env;

  InputHandler input_handler(&env);

  while (true) {
    // Flush after every std::cout / std:cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    std::cout << "$ "; // Print prompt

    std::string input;
    input_handler.readInput(input);
    if (input.empty())
      continue;

    input = utils::trim(input);

    if (input == "exit") {
      return 0;
    }

    const std::optional<utils::Command> command =
        utils::splitPreserveQuotedContent(input, ' ');

    if (!command) {
      continue;
    }

    const std::vector<std::string> &tokens = command->tokens;
    size_t tokens_size = tokens.size();

    const std::string &exe_command = tokens[0];

    DBG_EXEC(
        for (auto &&i : tokens) { std::cout << "token : " << i << "\n"; }

        for (auto &&i : command->redirects) {
          std::cout << "redirect : " << i.op << " " << i.filepath << " "
                    << i.file_descriptor << "\n";
        }

    // clang-format off
        #ifdef DEBUG_TOKEN
          DBG_EXIT(1);
        #endif
        // clang-format on
    );

    switch (CommandHandler::getBuiltinType(exe_command)) {
    case CommandHandler::BuiltinType::TYPE: {
      CommandHandler::handleType(*command, env);
      break;
    }

    case CommandHandler::BuiltinType::CD: {
      if (tokens_size > 2) {
        std::cerr << "cd: too many arguments\n";
      } else if (tokens_size == 2) {
        CommandHandler::changeDirectory(tokens[1]);
      }
      break;
    }

    case CommandHandler::BuiltinType::ECHO_CMD: {
      CommandHandler::handleEchoCommand(*command);
      break;
    }

    case CommandHandler::BuiltinType::PWD: {
      std::cout << std::filesystem::current_path().string() << "\n";
      break;
    }

    case CommandHandler::BuiltinType::EXIT: {
      if (tokens_size == 2 && utils::isNumber(tokens[1])) {
        return std::stoi(tokens[1]);
      }
      break;
    }

    case CommandHandler::BuiltinType::NOT_FOUND: {
      int status = CommandHandler::run(exe_command, *command, env);
      if (status == -1) {
        std::cout << input << ": command not found" << std::endl;
      }
      break;
    }
    }
  }
}
