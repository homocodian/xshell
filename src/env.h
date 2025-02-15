#ifndef ENV_CPP
#define ENV_CPP

#include <iostream>
#include <vector>
#include <string>
#include <optional>

namespace Env {
	std::optional<std::vector<std::string>> getDirs();

	std::optional<std::string> getExePath(const std::string& dir,
		const std::string& executable);
}

#endif