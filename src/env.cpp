#include <optional>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/stat.h> // For stat() function and S_IXUSR macro
#endif

#include "utils.h"

namespace Env {
std::optional<std::vector<std::string>> getDirs() {
  const char *path = getenv("PATH");

  if (path == NULL) {
    return std::nullopt;
  }

  const auto os = utils::getOS();

  if (os == utils::OS::Unknown) {
    utils::terminateProcessWithMessage("Error: Unknown OS");
  }

  if (os == utils::OS::Windows) {
    return utils::split(path, ';');
  } else {
    return utils::split(path, ':');
  }
}

bool isExecutable(const std::string &path) {
#ifdef _WIN32
  // Windows-specific check for executability
  DWORD file_Attr = GetFileAttributes((path + ".exe").c_str());

  if (file_Attr == INVALID_FILE_ATTRIBUTES) {
    return false; // File does not exist or other error
  }

  // Check if it's a file and if it has the executable attribute
  return (file_Attr & FILE_ATTRIBUTE_DIRECTORY) == 0;
#else
  // Unix-like systems (Linux/macOS) check for executable permission
  struct stat file_stat;
  if (stat(path.c_str(), &file_stat) == 0) {
    // Check if the file is executable (owner has execute permission)
    return (file_stat.st_mode & S_IXUSR) != 0;
  }
  return false;
#endif
}

std::optional<std::string> getExePath(const std::string &dir,
                                      const std::string &executable) {
  std::string file_path = dir;
  size_t slash_pos = dir.find_first_of("/\\");

  if (slash_pos != std::string::npos) {
    const char lastChar = dir.back();

    if (lastChar == '\\' || lastChar == '/') {
      file_path += executable;
    } else {
      file_path += dir[slash_pos];
      file_path += executable;
    }

    if (isExecutable(file_path)) {
      return file_path;
    }
  }

  return std::nullopt;
}
} // namespace Env
