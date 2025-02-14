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
    std::vector<std::string> tokens = split(input, ' ');

    if (tokens[0] == "exit" && tokens.size() == 2 && isNumber(tokens[1])) {
      exit(std::stoi(tokens[1]));
    }

    if (!input.empty()) {
      std::cout << input << ": command not found" << std::endl;
    }
  }
}
