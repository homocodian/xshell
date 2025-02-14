#include <iostream>
#include <sstream>
#include <vector>

std::string trim(const std::string& str) {
  size_t start = str.find_first_not_of(" \t\n\r\f\v");
  if (start == std::string::npos) {
    return "";
  }

  size_t end = str.find_last_not_of(" \t\n\r\f\v");

  return str.substr(start, end - start + 1);
}

bool isNumber(const std::string& str) {
  return !str.empty() &&
         str.find_first_not_of("-0123456789") == std::string::npos;
}

std::vector<std::string> split(const std::string& str, char delimiter) {
  std::vector<std::string> result;
  std::istringstream stream(str);
  std::string token;

  // Use getline to read each token separated by the delimiter
  while (std::getline(stream, token, delimiter)) {
    result.push_back(token);
  }

  return result;
}

bool contains(const char* const str[], size_t size, const std::string& value) {
  for (size_t i = 0; i < size; i++) {
    if (str[i] == value) {
      return true;
    }
  }
  return false;
}

int main() {
  const char* const commands[] = {"type", "echo", "exit"};
  const unsigned int no_of_commands = sizeof(commands) / sizeof(commands[0]);

  while (true) {
    // Flush after every std::cout / std:cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    // Uncomment this block to pass the first stage
    std::cout << "$ ";

    std::string input;
    std::getline(std::cin, input);

    input = trim(input);

    if (input == "exit") {
      exit(EXIT_SUCCESS);
    }

    const std::vector<std::string> tokens = split(input, ' ');
    size_t tokens_size = tokens.size();

    if (tokens[0] == "exit" && tokens.size() == 2 && isNumber(tokens[1])) {
      exit(std::stoi(tokens[1]));
    }

    if (tokens[0] == "echo") {
      for (size_t i = 1; i < tokens_size; i++) {
        std::cout << tokens[i] << ' ';
      }
      std::cout << "\n";
    }

    else if (tokens[0] == "type") {
      for (size_t i = 1; i < tokens_size; i++) {
        if (contains(commands, no_of_commands, tokens[i])) {
          std::cout << tokens[i] << " is a shell builtin\n";
        } else {
          std::cout << tokens[i] << ": not found" << std::endl;
        }
      }
    }

    else {
      if (!input.empty()) {
        std::cout << input << ": command not found" << std::endl;
      }
    }
  }
}
