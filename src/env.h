#ifndef ENV_H
#define ENV_H

#include <condition_variable>
#include <optional>
#include <string>
#include <thread>
#include <tsl/htrie_set.h>
#include <vector>

class Env {

private:
  std::vector<std::string> path_dirs;
  tsl::htrie_set<char> command_trie;
  std::thread completion_thread;
  std::mutex trie_mutex;
  std::condition_variable cv;
  bool is_command_trie_ready = false;

  void createCommandTrie();

public:
  std::vector<std::string> &getDirs();

  std::optional<std::string> getExePath(const std::string &dir,
                                        const std::string &command);
  Env &operator=(const Env &) = delete;

  std::optional<std::string> getFilePathFromPATH(const std::string &command);

  tsl::htrie_set<char> &getCommandTrie();

  Env();
  ~Env();
};
#endif
