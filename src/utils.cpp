#include "utils.h"

#include <charconv>
#include <iostream>
#include <optional>
#include <regex>
#include <string>
#include <vector>

std::string utils::trim(const std::string& str) {
  size_t start = str.find_first_not_of(" \t\n\r\f\v");
  if (start == std::string::npos) return "";
  size_t end = str.find_last_not_of(" \t\n\r\f\v");

  return str.substr(start, end - start + 1);
}

bool utils::isNumber(const std::string& str) {
  return !str.empty() &&
         str.find_first_not_of("-0123456789") == std::string::npos;
}

int safe_stoi(const std::string& str, int default_value = 1) {
  int result = 0;
  auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), result);
  if (ec == std::errc() && ptr == str.data() + str.size()) {
    return result;
  }
  return default_value;
}

std::vector<std::string> utils::split(std::string_view str, char delimiter) {
  std::vector<std::string> result;
  size_t start = 0, end;

  while ((end = str.find(delimiter, start)) != std::string_view::npos) {
    result.emplace_back(str.substr(start, end - start));
    start = end + 1;
  }
  result.emplace_back(str.substr(start));
  return result;
}

typedef struct Splitted_Token {
  std::string token;
  bool preserved;
} Splitted_Token;

// FIXME: doesn't parse '$' correctly
std::optional<utils::Command> utils::splitPreserveQuotedContent(
    const std::string& str, char delimiter) {
  std::vector<Splitted_Token> tokens;

  std::string token;
  bool in_single_quotes = false;
  bool in_double_quotes = false;
  bool escaped = false;
  size_t str_size = str.size();

  bool preversed = false;

  for (size_t i = 0; i < str_size; ++i) {
    char c = str[i];

    if (escaped) {
      token += c;
      escaped = false;
    } else if (c == '\\' && !in_single_quotes && in_double_quotes) {
      if (i + 1 < str_size) {
        char next_char = str[i + 1];
        if (next_char == '\\' || next_char == '$' || next_char == '\"' ||
            next_char == '\n') {
          token += str[++i];
        } else {
          token += c;
        }
      }
    } else if (c == '\\' && !in_single_quotes && !in_double_quotes) {
      escaped = true;
    } else if (c == '\'' && !in_double_quotes) {
      in_single_quotes = !in_single_quotes;
    } else if (c == '\"' && !in_single_quotes) {
      in_double_quotes = !in_double_quotes;
    } else if (c == delimiter && !in_single_quotes && !in_double_quotes) {
      if (!token.empty()) {
        tokens.emplace_back(Splitted_Token{token, preversed});
        preversed = false;
      }

      token.clear();
    } else {
      token += c;
    }

    if (in_single_quotes || in_double_quotes || escaped) {
      preversed = true;
    }
  }

  if (!token.empty()) {
    tokens.push_back(Splitted_Token{token, preversed});
  }

  utils::Command command;

  // std::regex redirect_regex(R"((\d*)(\>\>|\>\||\>)(.*?)(?=[ \"\']|$))");
  std::regex pattern(R"(>>|&>>|>|<|>\||&>|>&)");
  std::smatch matches;

  size_t tokens_size = tokens.size();

  for (int i = 0; i < tokens_size; ++i) {
    if (!tokens[i].preserved &&
        std::regex_search(tokens[i].token, matches, pattern)) {
      const std::string file_descriptor = matches.prefix().str();
      const std::string op = matches.str();
      std::string filepath = matches.suffix().str();

      if (filepath.empty()) {
        ++i;

        if (tokens_size < i) {
          std::cerr << "file or filepath is required to redirect\n";
          return std::nullopt;
        }

        std::smatch next_op_match;

        if (!tokens[i].preserved &&
            std::regex_search(tokens[i].token, next_op_match, pattern)) {
          std::cerr << "syntax error near unexpected token " << "\'"
                    << next_op_match.str() << "\'" << "\n";
          return std::nullopt;
        }

        filepath = tokens[i].token;
      }

      command.redirects.emplace_back(
          Redirect{.file_descriptor =
                       file_descriptor.empty() ? 1 : safe_stoi(file_descriptor),
                   .op = op,
                   .filepath = filepath});

      continue;
    }

    command.tokens.push_back(tokens[i].token);
  }

  return command;
}

void utils::exitWithMessage(const std::string& message) {
  std::cerr << message << std::endl;
  exit(EXIT_FAILURE);
}
