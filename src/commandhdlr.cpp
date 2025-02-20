#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#ifdef DEBUG
#include <cerrno>
#include <cstring>
#endif // DEBUG

#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#else
#include <unistd.h>
#include <wait.h>
#endif // _WIN32

#include "commandhdlr.h"
#include "env.h"
#include "errors.h"
#include "utils.h"

int CommandHandler::run(const std::string &command,
                        const std::vector<std::string> &args, Env &env)
{

  std::optional<std::string> filepath = env.getFilePathFromPATH(command);

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
  std::string command_line = *filepath;
  for (size_t i = 1; i < args.size(); ++i)
  {
    command_line += " " + args[i];
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
    return -1;
  }

  case 0:
  {

    std::vector<const char *> unix_args;

    for (auto &&it : args)
    {
      unix_args.emplace_back(it.c_str());
    }

    unix_args.push_back(nullptr);

    int err = execvp(command.c_str(), const_cast<char **>(unix_args.data()));

    // clang-format off
    #ifdef DEBUG
      std::cerr << "execvp failed: " << strerror(errno) << std::endl;
    #endif // DEBUG
    // clang-format on

    ErrorHandler::handleError(
        "[CommandHandler:run]: unreachable statement in child process");
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
  ErrorHandler::handleError("[CommandHandler::run]: unreachable");

#endif

  return -1;
}

void CommandHandler::handleType(
    const std::vector<std::string> &tokens,
    const std::vector<const char *> &builtin_commands, Env &env)
{

  for (size_t i = 1; i < tokens.size(); i++)
  {
    if (utils::contains(builtin_commands.data(), builtin_commands.size(),
                        tokens[i]))
    {
      std::cout << tokens[i] << " is a shell builtin\n";
    }
    else
    {

      std::optional<std::string> exe_path = env.getFilePathFromPATH(tokens[i]);

      if (exe_path)
      {
        std::cout << tokens[i] << " is " << *exe_path << "\n";
      }
      else
      {
        std::cout << tokens[i] << ": not found" << std::endl;
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
