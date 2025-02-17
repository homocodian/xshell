#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

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
                        const std::vector<std::string> &args) {

#ifdef _WIN32

  // Buffer to store the resolved program path
  char program_path[MAX_PATH];
  std::string exe_command = command + ".exe";

  // Search for the program in the directories listed in the PATH environment
  // variable
  DWORD result =
      SearchPath(NULL, exe_command.c_str(), NULL, MAX_PATH, program_path, NULL);

  if (result == 0) {
    // std::cerr << "Program not found in PATH." << std::endl;
    // std::cerr << "CreateProcess failed: " << GetLastError() << std::endl;
    return Error::NOT_FOUND;
  }

  HANDLE h_job = CreateJobObject(NULL, NULL);

  if (h_job == NULL) {
    return Error::CREATE_JOB;
  }

  // Set the job object to terminate all processes in it when the parent process
  // terminates
  JOBOBJECT_BASIC_LIMIT_INFORMATION job_limit = {0};
  job_limit.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
  JOBOBJECT_EXTENDED_LIMIT_INFORMATION job_info = {0};
  job_info.BasicLimitInformation = job_limit;

  if (!SetInformationJobObject(h_job, JobObjectExtendedLimitInformation,
                               &job_info, sizeof(job_info))) {
    return Error::SET_INFO_JOB_OBJECT;
  }

  // Combine the resolved program path and arguments into a single command
  // string
  std::string command_line = program_path;
  for (size_t i = 1; i < args.size(); ++i) {
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
  if (!CreateProcess(NULL, // Application name (NULL means command line is used)
                     cmd_line, // Command line to execute
                     NULL,     // Process attributes
                     NULL,     // Thread attributes
                     FALSE,    // Inherit handles
                     0,        // Creation flags
                     NULL,     // Environment variables
                     NULL,     // Current directory
                     &si,      // Startup information
                     &pi       // Process information
                     )) {
    // std::cerr << "CreateProcess failed: " << GetLastError() << std::endl;
    return Error::NO_CHILD_PROCESS;
  } else {

    if (!AssignProcessToJobObject(h_job, pi.hProcess)) {
      return Error::ASSIGN_PROCESS_TO_JOB;
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

  int child_pid = fork();

  if (child_pid == -1) {
    return Error::NO_CHILD_PROCESS;
  }

  if (child_pid == 0) {
    std::vector<const char *> unix_args;

    for (auto it = args.begin(); it != args.end(); ++it) {
      unix_args.emplace_back(it->c_str());
    }

    int err = execvp(command.c_str(), const_cast<char **>(unix_args.data()));

    if (err == -1) {
      // std::cerr << "execvp failed: " << strerror(errno) << std::endl;
      exit(-1); // Exit with an error code
    }

    return Error::NOT_FOUND;

  } else {
    int wstatus;
    wait(&wstatus);

    // std::cout << "waited for " << child_pid << " to finish with " << wstatus
    //           << "\n";

    if (WIFEXITED(wstatus) && WEXITSTATUS(wstatus) != 0) {
      return Error::NOT_FOUND;
    }
  }

  return 0;

#endif
}

void CommandHandler::handleType(const std::vector<std::string> &tokens,
                                const char *const builtin_commands[],
                                size_t builtin_commands_size, Env &env) {

  for (size_t i = 1; i < tokens.size(); i++) {
    if (utils::contains(builtin_commands, builtin_commands_size, tokens[i])) {
      std::cout << tokens[i] << " is a shell builtin\n";
    } else {

      auto exe_path = env.getExePathFromPATH(tokens[i]);

      if (exe_path) {
        std::cout << tokens[i] << " is " << *exe_path << "\n";
      } else {
        std::cout << tokens[i] << ": not found" << std::endl;
      }
    }
  }
}

void CommandHandler::changeDirectory(const std::string &path) {
  int status = -1;

#ifdef _WIN32

  if (path == "~") {
    const char *home_dir = std::getenv("USERPROFILE");

    if (home_dir == nullptr) {
      std::cerr << "cd: Unable to find home directory\n";
      return;
    }

    status = _chdir(home_dir);
  } else {
    status = _chdir(path.c_str());
  }

#elif defined(__unix__) || defined(__linux__) || defined(__APPLE__)

  if (path == "~") {
    const char *home_dir = std::getenv("HOME");

    if (home_dir == nullptr) {
      std::cerr << "cd: Unable to find home directory\n";
      return;
    }

    status = chdir(home_dir);
  } else {
    status = chdir(path.c_str());
  }

#else
  std::cerr << "Platform not supported" << std::endl;
  exit(EXIT_FAILURE);
#endif

  if (status == -1) {
    std::cerr << "cd: " << path << ": "
              << "No such file or directory" << std::endl;
  }
}
