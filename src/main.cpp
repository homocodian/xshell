#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "commandhdlr.h"
#include "env.h"
#include "utils.h"

int main() {
  const std::vector<const char *> builtin_commands = {"type", "echo", "exit",
                                                      "pwd", "cd"};

  if (utils::getOS() == utils::OS::Unknown) {
    std::cerr << "Unknown plaftform, not supported\n";
    return 1;
  }

  Env env;

  while (true) {
    // Flush after every std::cout / std:cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    // Uncomment this block to pass the first stage
    std::cout << "$ ";

    std::string input;
    std::getline(std::cin, input);

    if (input.empty()) {
      continue;
    }

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

    // clang-format off
    #if  defined (DEBUG_TOKEN) || defined (DEBUG)
      for (auto &&i : tokens) {
        std::cout << "token : " << i << "\n";
      }

      for (auto &&i : command->redirects) {
        std::cout << "redirect : " << i.op << " "  << i.filepath << " " << i.file_descriptor << "\n";
      }

      #ifdef DEBUG_TOKEN
        continue;
      #endif
    #endif
    // clang-format on

    if (exe_command == "exit" && tokens_size == 2 &&
        utils::isNumber(tokens[1])) {
      exit(std::stoi(tokens[1]));
    }

    if (exe_command == "echo") {
      CommandHandler::handleEchoCommand(*command);
    }

    else if (exe_command == "type") {
      CommandHandler::handleType(*command, builtin_commands, env);
    }

    else if (exe_command == "pwd") {
      std::cout << std::filesystem::current_path().string() << "\n";
    }

    else if (exe_command == "cd") {
      if (tokens_size > 2) {
        std::cerr << "cd: too many arguments\n";
      } else if (tokens_size == 2) {
        CommandHandler::changeDirectory(tokens[1]);
      }
    }

    else {
      int status = CommandHandler::run(exe_command, *command, env);

      // clang-format off
      #ifdef DEBUG
        std::cerr << "DEBUGPRINT[6]: main.cpp:81: status=" << status << std::endl;
      #endif // DEBUG
      // clang-format on

      if (status == -1) {
        std::cout << input << ": command not found" << std::endl;
      }
    }
  }
}
