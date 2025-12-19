#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#ifdef DEBUG
#include <iostream>
#endif

#include "commandhdlr.h"
#include "env.h"
#include "utils.h"

#ifdef _WIN32
constexpr char PATH_DELIMITER = ';';
#else
constexpr char PATH_DELIMITER = ':';
#endif

const std::vector<std::string> Env::getDirs() const {
  const char* path = getenv("PATH");

  if (!path) return {};

  return utils::split(std::string_view(path), PATH_DELIMITER);
}

std::optional<std::string> Env::getExePath(std::string_view dir,
                                           const std::string& command) const {
  std::string file_path(dir);
  size_t slash_pos = dir.find_first_of("/\\");

  if (slash_pos != std::string::npos) {
    const char lastChar = dir.back();

    if (lastChar == '\\' || lastChar == '/') {
      file_path += command;
    } else {
      file_path += dir[slash_pos];
      file_path += command;
    }

#ifdef _WIN32
    // 4 is read permission

    std::string exe_file = file_path + ".exe";
    std::string cmd_file = file_path + ".cmd";

    if (_access(exe_file.c_str(), 4) == 0) {
      return exe_file;
    } else if (_access(cmd_file.c_str(), 4) == 0) {
      return cmd_file;
    }
#else
    if (access(file_path.c_str(), R_OK | X_OK) == 0) {
      return file_path;
    }
#endif  // _WIN32
  }

  return std::nullopt;
}

std::optional<std::string> Env::getFilePathFromPATH(
    const std::string& command) const {
  std::optional<std::string> exe_path = std::nullopt;

  const auto& dirs = this->getDirs();

  for (const auto& dir : dirs) {
    exe_path = this->getExePath(dir, command);

    if (exe_path) {
      return exe_path;
    }
  }

  return std::nullopt;
}
