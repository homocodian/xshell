#include "input_handler.h"

#include <iostream>
#include <string>

#include "commandhdlr.h"
#include "env.h"

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>

#include <cstdio>
#endif  // _WIN32

char InputHandler::getKey() {
#ifdef _WIN32
  return _getch();
#endif
  return std::getchar();
}

// enable raw mode in terminal
void InputHandler::enableRawMode() {
#ifdef _WIN32

  HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
  GetConsoleMode(hStdin, &originalConsoleMode);

  DWORD rawMode =
      originalConsoleMode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
  SetConsoleMode(hStdin, rawMode);

#else

  tcgetattr(STDIN_FILENO, &this->oldt);    // Save current terminal settings
  this->newt = oldt;                       // Copy the settings
  this->newt.c_lflag &= ~(ICANON | ECHO);  // Disable canonical mode and echo
  this->newt.c_cc[VMIN] = 1;   // Minimum number of characters to read
  this->newt.c_cc[VTIME] = 0;  // Timeout in deciseconds
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);  // Set new terminal settings

#endif  // _WIN32
}

// Restore original terminal settings
void InputHandler::disableRawMode() {
#ifdef _WIN32

  HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
  SetConsoleMode(hStdin, originalConsoleMode);

#else

  tcsetattr(STDIN_FILENO, TCSANOW, &this->oldt);

#endif  // DEBUG
}

constexpr inline void alert_terminal() { std::cout << "\a"; }

const char TAB = '\t';
const char ESC = 27;
const char DEL = 127;
const char ENTER = '\n';

// FIXME: multiline editing is not supported yet
// such as deleting characters wraps into multiple line
void InputHandler::readInput(std::string& input_buffer) {
  char ch = 0;
  size_t cursor_pos = 0;

  bool is_newline = false;

  char previous_pressed_key = 0;

  CommandHandler::Completion completion;

  enableRawMode();

  while (!is_newline) {
    ch = getKey();

    switch (ch) {
      case TAB: {
        if (cursor_pos > 0) {
          if (previous_pressed_key == TAB &&
              completion.getCompletionSize() > 1 &&
              completion.getCompletionOf() == input_buffer) {
            disableRawMode();
            completion.printLastCompletions();
            std::cout << "\n$ " << input_buffer;
            enableRawMode();
            break;
          }

          size_t start_of_word = input_buffer.rfind(' ', cursor_pos - 1);
          if (start_of_word == std::string::npos) {
            start_of_word = 0;  // only one word exists
          } else {
            start_of_word++;  // start of the word is next character after space
          }

          std::string_view current_word = std::string_view(
              input_buffer.data() + start_of_word, cursor_pos - start_of_word);

          if (!current_word.empty()) {
            auto particial_completion =
                completion.getCompletion(current_word, env);

            if (particial_completion.empty()) {
              alert_terminal();
              break;
            }

            int delete_length = cursor_pos - start_of_word;

            // Delete characters backwards
            for (int i = 0; i < delete_length; ++i) {
              // Move cursor left by one
              // Delete the character there
              std::cout << "\033[D\033[P";
            }

            input_buffer.erase(start_of_word, delete_length);
            input_buffer.insert(start_of_word, particial_completion);
            cursor_pos = start_of_word + particial_completion.size();

            std::cout << input_buffer.substr(start_of_word);

            if (cursor_pos < input_buffer.size()) {
              // Move cursor back to the original position
              // after printing the rest of the string
              std::cout << "\033[" << input_buffer.size() - cursor_pos << "D";
            }

            if (!particial_completion.empty() &&
                particial_completion.back() != ' ') {
              alert_terminal();
            }

            break;
          }
        }
        alert_terminal();
        break;
      }
      case ESC: {
        ch = getKey();
        if (ch == 91) {
          ch = getKey();
          if (ch == 67 &&
              cursor_pos <
                  input_buffer.size()) {  // Right Arrow Key (ASCII 67) or ESC[C
            cursor_pos++;
            std::cout << "\033[C";  // Move terminal's cursor right
          } else if (ch == 68 &&
                     cursor_pos > 0) {  // Left Arrow Key (ASCII 68) or ESC[D
            cursor_pos--;
            std::cout << "\033[D";  // Move terminal's cursor left
          }
        }

        break;
      }
      case DEL: {
        if (cursor_pos > 0) {
          input_buffer.erase(--cursor_pos, 1);
          std::cout << "\033[D\033[P";  // move left and delete character.
        }

        break;
      }
      case ENTER: {
        std::cout << "\n";
        is_newline = true;
        break;
      }
      default: {
        input_buffer.insert(cursor_pos++, 1, ch);
        std::cout << ch;
        if (cursor_pos < input_buffer.size()) {
          std::cout << input_buffer.substr(cursor_pos);
          std::cout << "\033[" << input_buffer.size() - cursor_pos << "D";
        }
        break;
      }
    }
    previous_pressed_key = ch;
  }
  disableRawMode();
}

InputHandler::InputHandler(Env* env) : env(env) {}
