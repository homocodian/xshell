#include <filesystem>
#include <optional>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/stat.h> // For stat() function and S_IXUSR macro
#endif

#include "env.h"
#include "utils.h"

std::vector<std::string> &Env::getDirs() {

  if (!this->path_dirs.empty()) {
    return this->path_dirs;
  }

  const char *path = getenv("PATH");

  if (path == NULL) {
    return this->path_dirs;
  }

  const auto os = utils::getOS();

  if (os == utils::OS::Unknown) {
    utils::exitWithMessage("Error: Unknown OS");
  }

  if (os == utils::OS::Windows) {
    return this->path_dirs = utils::split(path, ';');
  } else {
    return this->path_dirs = utils::split(path, ':');
  }
}

std::optional<std::string> Env::getExePath(const std::string &dir,
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

#ifdef _WIN32
    file_path += ".exe";

    if (std::filesystem::exists(file_path)) {
      return file_path
    }
#else
    if (std::filesystem::exists(file_path)) {
      return file_path;
    }
#endif // _WIN32
  }

  return std::nullopt;
}

std::optional<std::string> Env::getFilePathFromPATH(const std::string &exe) {
  std::optional<std::string> exe_path = std::nullopt;

  auto dirs = this->getDirs();
  for (auto &&dir : dirs) {
    exe_path = this->getExePath(dir, exe);

    if (exe_path) {
      return exe_path;
    }
  }

  return std::nullopt;
}
