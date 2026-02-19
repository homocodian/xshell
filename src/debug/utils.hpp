#pragma once

#ifdef DEBUG
#include <format>
#include <iostream>

#define DBG_PRINT(fmt, ...)                                               \
  (std::cerr << std::format("[DEBUG_PRINT:{}:{}:{}] " fmt "\n", __FILE__, \
                            __LINE__, __func__, ##__VA_ARGS__)            \
             << std::flush)

#define DBG_EXEC(code)                                                      \
  do {                                                                      \
    std::cerr << std::format("[DEBUG_EXEC:{}:{}:{}]\n", __FILE__, __LINE__, \
                             __func__)                                      \
              << std::flush;                                                \
    code;                                                                   \
  } while (0)

#include <cstdlib>
#define DBG_EXIT(code)                                                       \
  do {                                                                       \
    std::cerr << std::format("[DEBUG_EXIT:{}:{}:{}] Exiting with code {}\n", \
                             __FILE__, __LINE__, __func__, code)             \
              << std::flush;                                                 \
    std::exit(code);                                                         \
  } while (0)

#else

#define DBG_PRINT(fmt, ...) ((void)0)
#define DBG_EXEC(code) ((void)0)
#define DBG_EXIT(code) ((void)0)

#endif
