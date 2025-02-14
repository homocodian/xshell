#include <iostream>

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

int getExitCode(const std::string& str) {
  if (str.size() < 6) {
    throw std::invalid_argument("Input string is too short.");
  }

  // exit + 1 whitespace -> 0-4 char
  std::string exit_code = str.substr(5);

  if (!isNumber(exit_code)) {
    throw std::invalid_argument("Extracted exit code is not a valid number.");
  }

  try {
    return std::stoi(exit_code);
  } catch (const std::invalid_argument&) {
    throw std::invalid_argument(
        "The exit code cannot be converted to an integer.");
  } catch (const std::out_of_range&) {
    throw std::out_of_range(
        "The exit code is out of the range for an integer.");
  }
}

int main() {
  while (true) {
    // Flush after every std::cout / std:cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    // Uncomment this block to pass the first stage
    std::cout << "$ ";

    std::string input;
    std::getline(std::cin, input);

    input = trim(input);

    if (input.find("exit ") != std::string::npos) {
      try {
        int exit_code = getExitCode(input);
        exit(exit_code);
      } catch (const std::exception& e) {
      }
    }

    if (!input.empty()) {
      std::cout << input << ": command not found" << std::endl;
    }
  }
}
