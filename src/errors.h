#ifndef ERRORS_H
#define ERRORS_H

#include <string>

class ErrorHandler {
public:
  static void handleError(const std::string &message);
};

#endif // !ERRORS_H
