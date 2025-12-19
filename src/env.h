#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

class Env {
 private:
 public:
  const std::vector<std::string> getDirs() const;

  std::optional<std::string> getExePath(std::string_view dir,
                                        const std::string& command) const;
  Env& operator=(const Env&) = delete;

  std::optional<std::string> getFilePathFromPATH(
      const std::string& command) const;
};
