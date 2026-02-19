#include "error/errors.hpp"
#include <iostream>

void ErrorHandler::handleError(const std::string &message) {
  std::cerr << "Error: " << message << std::endl;
  std::exit(EXIT_FAILURE);
}
