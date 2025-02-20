#include <optional>
#include <string>
#include <vector>

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#include "env.h"
#include "utils.h"

std::vector<std::string> &Env::getDirs()
{

  if (!this->path_dirs.empty())
  {
    return this->path_dirs;
  }

  const char *path = getenv("PATH");

  if (path == NULL)
  {
    return this->path_dirs;
  }

  const auto os = utils::getOS();

  if (os == utils::OS::Unknown)
  {
    utils::exitWithMessage("Error: Unknown OS");
  }

  if (os == utils::OS::Windows)
  {
    return this->path_dirs = utils::split(path, ';');
  }
  else
  {
    return this->path_dirs = utils::split(path, ':');
  }
}

std::optional<std::string> Env::getExePath(const std::string &dir,
                                           const std::string &command)
{
  std::string file_path = dir;
  size_t slash_pos = dir.find_first_of("/\\");

  if (slash_pos != std::string::npos)
  {
    const char lastChar = dir.back();

    if (lastChar == '\\' || lastChar == '/')
    {
      file_path += command;
    }
    else
    {
      file_path += dir[slash_pos];
      file_path += command;
    }

#ifdef _WIN32
    // 4 is read permission

    std::string exe_file = file_path + ".exe";
    std::string cmd_file = file_path + ".cmd";

    if (_access(exe_file.c_str(), 4) == 0)
    {
      return exe_file;
    }
    else if (_access(cmd_file.c_str(), 4) == 0)
    {
      return cmd_file;
    }
#else
    if (access(file_path.c_str(), R_OK | X_OK) == 0)
    {
      return file_path;
    }
#endif // _WIN32
  }

  return std::nullopt;
}

std::optional<std::string>
Env::getFilePathFromPATH(const std::string &command)
{
  std::optional<std::string> exe_path = std::nullopt;

  auto dirs = this->getDirs();

  for (auto &&dir : dirs)
  {
    exe_path = this->getExePath(dir, command);

    if (exe_path)
    {
      return exe_path;
    }
  }

  return std::nullopt;
}
