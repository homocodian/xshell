#include <cstddef>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "commandhdlr.h"
#include "env.h"
#include "errors.h"
#include "utils.h"

int main() {
  const std::vector<const char *> builtin_commands = {"type", "echo", "exit",
                                                      "pwd", "cd"};

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
      exit(EXIT_SUCCESS);
    }

    const std::vector<std::string> tokens =
        utils::splitPreserveQuotedContent(input, ' ');
    size_t tokens_size = tokens.size();
    const std::string &command = tokens[0];

#ifdef DEBUG
    for (auto &&i : tokens) {
      std::cout << i << "\n";
    }
#endif // DEBUG

    if (command == "exit" && tokens.size() == 2 && utils::isNumber(tokens[1])) {
      exit(std::stoi(tokens[1]));
    }

    if (command == "echo") {
      for (size_t i = 1; i < tokens_size; i++) {
        std::cout << tokens[i] << ' ';
      }
      std::cout << "\n";
    }

    else if (command == "type") {
      CommandHandler::handleType(tokens, builtin_commands, env);
    }

    else if (command == "pwd") {
      std::cout << std::filesystem::current_path().string() << "\n";
    }

    else if (command == "cd") {
      if (tokens_size > 2) {
        std::cerr << "cd: too many arguments\n";
      } else if (tokens_size == 2) {
        CommandHandler::changeDirectory(tokens[1]);
      }
    }

    else {
      int status_code = CommandHandler::run(command, tokens);
#ifdef DEBUG
      std::cerr << "DEBUGPRINT[2]: main.cpp:78: status_code=" << status_code
                << std::endl;
#endif // DEBUG
      if (status_code == Error::NOT_FOUND) {
        std::cout << input << ": command not found" << std::endl;
      }
    }
  }
}
