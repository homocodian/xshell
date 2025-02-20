#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "utils.h"

std::string utils::trim(const std::string &str) {
  size_t start = str.find_first_not_of(" \t\n\r\f\v");

  if (start == std::string::npos) {
    return "";
  }

  size_t end = str.find_last_not_of(" \t\n\r\f\v");

  return str.substr(start, end - start + 1);
}

bool utils::contains(const char *const str[], size_t size,
                     const std::string &value) {
  for (size_t i = 0; i < size; i++) {
    if (str[i] == value) {
      return true;
    }
  }
  return false;
}

bool utils::isNumber(const std::string &str) {
  return !str.empty() &&
         str.find_first_not_of("-0123456789") == std::string::npos;
}

std::vector<std::string> utils::split(const std::string &str, char delimiter) {
  std::vector<std::string> result;
  std::istringstream stream(str);
  std::string token;

  // Use getline to read each token separated by the delimiter
  while (std::getline(stream, token, delimiter)) {
    result.push_back(token);
  }

  return result;
}

std::vector<std::string>
utils::splitPreserveQuotedContent(const std::string &str, char delimiter) {
  std::vector<std::string> result;
  std::string token;
  bool in_single_quotes = false;
  bool in_double_quotes = false;
  bool escaped = false; // Flag to track escape sequences
  size_t str_size = str.size();

  for (size_t i = 0; i < str_size; ++i) {
    char c = str[i];

    if (escaped) { // Handle the escaped character, just skip the backslash
      token += c;
      escaped = false;
    } else if (c == '\\' && !in_single_quotes && in_double_quotes) {
      // Only process escape sequences in double quotes
      if (i + 1 < str_size) {
        char next_char = str[i + 1];
        if (next_char == '\\' || next_char == '$' || next_char == '\"' ||
            next_char == '\n') {
          // Skip the backslash and add the next character to the token
          token += str[++i]; // Skip the escape character itself and add the
                             // next character
        } else {
          token +=
              c; // Just add the backslash if not followed by special characters
        }
      }
    } else if (c == '\\' && !in_single_quotes &&
               !in_double_quotes) { // Start of escape sequence outside quotes
      escaped = true;
    } else if (c == '\'' && !in_double_quotes) { // Toggle single quote
      in_single_quotes = !in_single_quotes;
    } else if (c == '\"' && !in_single_quotes) { // Toggle double quote
      in_double_quotes = !in_double_quotes;
    } else if (c == delimiter && !in_single_quotes &&
               !in_double_quotes) { // Split on delimiter outside quotes
      if (!token.empty()) {
        result.push_back(token);
      }
      token.clear();
    } else {
      token += c; // Add regular characters
    }
  }

  if (!token.empty()) {
    result.push_back(token); // Push the last token
  }

  return result;
}

utils::OS utils::getOS() {
#if defined(_WIN32) || defined(_WIN64)
  return utils::Windows;
#elif defined(__linux__) || defined(unix) || defined(__unix__) ||              \
    defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
  return utils::Unix;
#else
  return utils::Unknown;
#endif
}

void utils::exitWithMessage(const std::string &message) {
  std::cerr << message << std::endl;
  exit(EXIT_FAILURE);
}
