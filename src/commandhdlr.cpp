#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#else
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#include <wait.h>
#endif // _WIN32

#include "commandhdlr.h"
#include "env.h"
#include "errors.h"
#include "utils.h"

int CommandHandler::run(const std::string &exe_command,
                        CommandHandler::command_t &command, Env &env)
{

  std::optional<std::string> filepath = env.getFilePathFromPATH(exe_command);

  if (!filepath)
  {
    return -1;
  }

#ifdef _WIN32

  HANDLE h_job = CreateJobObject(NULL, NULL);

  if (h_job == NULL)
  {
    return -1;
  }

  // Set the job object to terminate all processes in it when the parent process
  // terminates
  JOBOBJECT_BASIC_LIMIT_INFORMATION job_limit = {0};
  job_limit.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
  JOBOBJECT_EXTENDED_LIMIT_INFORMATION job_info = {0};
  job_info.BasicLimitInformation = job_limit;

  if (!SetInformationJobObject(h_job, JobObjectExtendedLimitInformation,
                               &job_info, sizeof(job_info)))
  {
    return -1;
  }

  // Combine the resolved program path and arguments into a single command
  // string
  size_t tokens_size = command.tokens.size();
  std::string command_line = *filepath;
  for (size_t i = 1; i < tokens_size; ++i)
  {
    command_line += ' ' + command.tokens[i];
  }

  // Convert the commandLine to a mutable char array (LPSTR)
  LPSTR cmd_line = const_cast<LPSTR>(command_line.c_str());

  // Setup the process information structures
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  ZeroMemory(&pi, sizeof(pi));

  // Create the new process
  if (!CreateProcess(NULL,     // Application name (NULL means command line is used)
                     cmd_line, // Command line to execute
                     NULL,     // Process attributes
                     NULL,     // Thread attributes
                     FALSE,    // Inherit handles
                     0,        // Creation flags
                     NULL,     // Environment variables
                     NULL,     // Current directory
                     &si,      // Startup information
                     &pi       // Process information
                     ))
  {
    // clang-format off
    #ifdef DEBUG
    // std::cerr << "CreateProcess failed: " << GetLastError() << std::endl;
    #endif
    // clang-format on
    return -1;
  }
  else
  {

    if (!AssignProcessToJobObject(h_job, pi.hProcess))
    {
      return -1;
    }

    // std::cout << "Process created successfully!" << std::endl;
    // Wait for the process to finish
    WaitForSingleObject(pi.hProcess, INFINITE);
    // std::cout << "Child process completed!" << std::endl;

    // Close the handles
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(h_job);

    return 0;
  }

#else

  int pid = fork();

  switch (pid)
  {

  case -1:
  {
    ErrorHandler::handleError("Out of memory");
    break;
  }

  case 0:
  {

    size_t tokens_size = command.tokens.size();
    size_t redirects_size = command.redirects.size();

    bool error = false;
    std::string error_message;

    int std_err_fd = dup(STDERR_FILENO);

    if (std_err_fd == -1)
    {
      std::cerr << "Failed to duplicate a STDERR_FILENO in child process\n";
      exit(errno);
    }

    // TODO: refactor
    if (tokens_size > 1 && redirects_size > 0)
    {
      for (auto &&redirect : command.redirects)
      {

        bool redirect_both = redirect.op.contains("&");

        int flags = redirect.op.contains(">>") ? O_APPEND | O_CREAT | O_WRONLY
                                               : O_WRONLY | O_CREAT | O_TRUNC;

        int file = open(redirect.filepath.c_str(), flags, 0644);

        if (file == -1)
        {
          std::string message =
              redirect.filepath + ": " + strerror(errno) + "\n";
          write(std_err_fd, message.c_str(), message.length());
          exit(errno);
        }

        if (!redirect_both)
        {

          if (dup2(file, redirect.file_descriptor) == -1)
          {
            std::string message = "Failed to redirect (" + redirect.op +
                                  ") to file: " + strerror(errno) + "\n";
            write(std_err_fd, message.c_str(), message.length());
            exit(errno);
          }
        }
        else
        {
          if (dup2(file, STDOUT_FILENO) == -1)
          {
            std::string message = "Failed to redirect (" + redirect.op +
                                  ") to file: " + strerror(errno) + "\n";
            write(std_err_fd, message.c_str(), message.length());
            exit(errno);
          }

          if (dup2(file, STDERR_FILENO) == -1)
          {
            std::string message = "Failed to redirect (" + redirect.op +
                                  ") to file: " + strerror(errno) + "\n";
            write(std_err_fd, message.c_str(), message.length());
            exit(errno);
          }
        }
      }
    }

    std::vector<const char *> unix_args;

    unix_args.reserve(tokens_size + 1);

    for (int i = 0; i < tokens_size; ++i)
    {
      unix_args.emplace_back(command.tokens[i].c_str());
    }

    unix_args[tokens_size] = nullptr;

#ifdef DEBUG
    for (size_t i = 0; i <= tokens_size; ++i)
    {
      if (unix_args[i] == nullptr)
      {
        std::cout << "nullptr" << "\n";
      }
      else
      {
        std::cout << unix_args[i] << "\n";
      }
    }
#endif // DEBUG

    int err =
        execvp(exe_command.c_str(), const_cast<char **>(unix_args.data()));

    // clang-format off
    #ifdef DEBUG
      std::cerr << "execvp failed: " << strerror(errno) << std::endl;
    #endif // DEBUG
    // clang-format on
    ErrorHandler::handleError("[CommandHandler::run:226]: command not found: "
                              "unreachable in child process");
  }

  default:
  {

    int wstatus;
    wait(&wstatus);

    // clang-format off
    #ifdef DEBUG
      std::cout << "waited for " << pid << " to finish with " << wstatus
              << " child process status " << WEXITSTATUS(wstatus) << "\n";
    #endif // DEBUG
    // clang-format on

    return WEXITSTATUS(wstatus);
  }
  }
  ErrorHandler::handleError("[CommandHandler::run:227]: unreachable");

#endif

  return -1;
}

void writeTo(const std::vector<std::string> &tokens, size_t tokens_size,
             auto &to)
{
  int index = tokens_size - 1;

  int i = 1;

  for (; i < index; ++i)
  {
    to << tokens[i] << ' ';
  }

  to << tokens[i] << "\n";
}

void CommandHandler::handleEchoCommand(command_t &command)
{

  const size_t tokens_size = command.tokens.size();
  const size_t redirects_size = command.redirects.size();

  bool already_written = false;

  if (tokens_size > 1 && redirects_size > 0)
  {
    for (auto it = command.redirects.rbegin(); it != command.redirects.rend();
         ++it)
    {

      if (it->filepath.empty())
      {
        std::cerr << "no file or path name provided\n";
        return;
      }

      std::ofstream file(it->filepath,
                         it->op.contains(">>") ? std::ios::app : std::ios::out);

      if (!file.is_open())
      {
        std::cerr << it->filepath << ": no such file or directory\n";
        return;
      }

      if (it->file_descriptor == 1)
      {
        already_written = true;
        writeTo(command.tokens, tokens_size, file);
      }
    }
  }

  if (!already_written)
  {
    writeTo(command.tokens, tokens_size, std::cout);
  }
}

void CommandHandler::handleType(const utils::Command &command, Env &env)
{

  const size_t tokens_size = command.tokens.size();

  for (size_t i = 1; i < tokens_size; ++i)
  {
    if (std::find(CommandHandler::builtin_commands.begin(),
                  CommandHandler::builtin_commands.end(), command.tokens[i]) !=
        CommandHandler::builtin_commands.end())
    {

      std::cout << command.tokens[i] << " is a shell builtin\n";
    }
    else
    {

      std::optional<std::string> exe_path =
          env.getFilePathFromPATH(command.tokens[i]);

      if (exe_path)
      {
        std::cout << command.tokens[i] << " is " << *exe_path << "\n";
      }
      else
      {
        std::cout << command.tokens[i] << ": not found" << std::endl;
      }
    }
  }
}

void CommandHandler::changeDirectory(const std::string &path)
{
  int status = -1;

#ifdef _WIN32

  if (path == "~")
  {
    const char *home_dir = std::getenv("USERPROFILE");

    if (home_dir == nullptr)
    {
      std::cerr << "cd: Unable to find home directory\n";
      return;
    }

    status = _chdir(home_dir);
  }
  else
  {
    status = _chdir(path.c_str());
  }

#elif defined(__unix__) || defined(__linux__) || defined(__APPLE__)

  if (path == "~")
  {
    const char *home_dir = std::getenv("HOME");

    if (home_dir == nullptr)
    {
      std::cerr << "cd: Unable to find home directory\n";
      return;
    }

    status = chdir(home_dir);
  }
  else
  {
    status = chdir(path.c_str());
  }

#else
  std::cerr << "Platform not supported" << std::endl;
  exit(EXIT_FAILURE);
#endif

  if (status == -1)
  {
    std::cerr << "cd: " << path << ": "
              << "No such file or directory" << std::endl;
  }
}

std::optional<std::string_view>
CommandHandler::searchCommand(const std::string_view &prefix)
{

  std::optional<std::string_view> result;
  size_t best_match_length = 0;

  for (const auto &command : CommandHandler::builtin_commands)
  {
    if (command.rfind(prefix, 0) == 0)
    {
      if (!result.has_value() || command.length() < best_match_length)
      {
        result = std::string_view(command);
        best_match_length = command.length();
      }
    }
  }

  return result;
}
