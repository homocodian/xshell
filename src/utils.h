#ifndef UTILS_CPP
#define UTILS_CPP

#include <iostream>
#include <string>
#include <vector>

namespace utils {
enum OS { Windows, Unix, Unknown };

std::string trim(const std::string &str);

bool contains(const char *const str[], size_t size, const std::string &value);

std::vector<std::string> split(const std::string &str, char delimiter);

bool isNumber(const std::string &str);

void terminateProcessWithMessage(const std::string &message);

enum OS getOS();
} // namespace utils

#endif
