#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include "env.h"
#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <termios.h> // Unix-specific headers
#include <unistd.h>
#endif

class InputHandler {
public:
  void readInput(std::string &input_buffer);
  InputHandler(Env *);

private:
  Env *env;
  char getKey();
  void enableRawMode();
  void disableRawMode();

#ifdef _WIN32
  DWORD originalConsoleMode;
#else
  struct termios newt, oldt;
#endif //  _WIN32
};

#endif
