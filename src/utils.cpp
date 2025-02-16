#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace utils {
std::string trim(const std::string &str) {
  size_t start = str.find_first_not_of(" \t\n\r\f\v");
  if (start == std::string::npos) {
    return "";
  }

  size_t end = str.find_last_not_of(" \t\n\r\f\v");

  return str.substr(start, end - start + 1);
}

bool contains(const char *const str[], size_t size, const std::string &value) {
  for (size_t i = 0; i < size; i++) {
    if (str[i] == value) {
      return true;
    }
  }
  return false;
}

bool isNumber(const std::string &str) {
  return !str.empty() &&
         str.find_first_not_of("-0123456789") == std::string::npos;
}

std::vector<std::string> split(const std::string &str, char delimiter) {
  std::vector<std::string> result;
  std::istringstream stream(str);
  std::string token;

  // Use getline to read each token separated by the delimiter
  while (std::getline(stream, token, delimiter)) {
    result.push_back(token);
  }

  return result;
}

enum OS { Windows, Unix, Unknown };

enum OS getOS() {
#if defined(_WIN32) || defined(_WIN64)
  return Windows;
#elif defined(__linux__) || defined(unix) || defined(__unix__) ||              \
    defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
  return Unix;
#else
  return Unknown;
#endif
}

void terminateProcessWithMessage(const std::string &message) {
  std::cerr << message << std::endl;
  exit(EXIT_FAILURE);
}

} // namespace utils
