#pragma once

#include <optional>
#include <string>
#include <vector>

namespace utils {
enum OS { Windows, Unix, Unknown };

struct Redirect {
  int file_descriptor = 1;
  std::string op;
  std::string filepath;
};

struct Command {
  std::vector<std::string> tokens;
  std::vector<Redirect> redirects;
};

std::string trim(const std::string& str);

std::vector<std::string> split(std::string_view str, char delimiter);

std::optional<Command> splitPreserveQuotedContent(const std::string& str,
                                                  char delimiter);

bool isNumber(const std::string& str);

void exitWithMessage(const std::string& message);

}  // namespace utils
