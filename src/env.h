#ifndef ENV_H
#define ENV_H

#include <iostream>
#include <optional>
#include <string>
#include <vector>

class Env {

private:
  std::vector<std::string> path_dirs;
  bool isExecutable(const std::string &path);

public:
  std::vector<std::string> &getDirs();

  std::optional<std::string> getExePath(const std::string &dir,
                                        const std::string &executable);
  Env &operator=(const Env &) = delete;

  std::optional<std::string> getExePathFromPATH(const std::string &exe);
};
#endif
