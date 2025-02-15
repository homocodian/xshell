#include <iostream>

#include "env.h"
#include "utils.h"

int main(int argc, const char* argv[]) {
	const char* const commands[] = { "type", "echo", "exit" };
	const unsigned int no_of_commands = sizeof(commands) / sizeof(commands[0]);
	std::vector<std::string> path_dirs;

	while (true) {
		// Flush after every std::cout / std:cerr
		std::cout << std::unitbuf;
		std::cerr << std::unitbuf;

		// Uncomment this block to pass the first stage
		std::cout << "$ ";

		std::string input;
		std::getline(std::cin, input);

		input = utils::trim(input);

		if (input == "exit") {
			exit(EXIT_SUCCESS);
		}

		const std::vector<std::string> tokens = utils::split(input, ' ');
		size_t tokens_size = tokens.size();

		if (tokens[0] == "exit" && tokens.size() == 2 &&
			utils::isNumber(tokens[1])) {
			exit(std::stoi(tokens[1]));
		}

		if (tokens[0] == "echo") {
			for (size_t i = 1; i < tokens_size; i++) {
				std::cout << tokens[i] << ' ';
			}
			std::cout << "\n";
		}

		else if (tokens[0] == "type") {

			for (size_t i = 1; i < tokens_size; i++) {
				bool found = false;
				if (utils::contains(commands, no_of_commands, tokens[i])) {
					std::cout << tokens[i] << " is a shell builtin\n";
				}
				else {
					// get and cache path dirs
					if (path_dirs.empty()) {
						auto dirs = Env::getDirs();
						if (dirs) {
							path_dirs = *dirs;
						}
					}

					if (!path_dirs.empty()) {
						for (auto&& dir : path_dirs) {
							auto exe_path = Env::getExePath(dir, tokens[i]);

							if (exe_path) {
								std::cout << tokens[i] << " is " << *exe_path << "\n";
								found = true;
								break;
							}
						}
					}
					if (!found) {
						std::cout << tokens[i] << ": not found" << std::endl;
					}
				}
			}
		}

		else {
			if (!input.empty()) {
				std::cout << input << ": command not found" << std::endl;
			}
		}
	}
}
