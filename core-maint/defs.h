// Author: Yikai Zhang
//
// This file contains some useful macros.
#ifndef CORE_DEFS_H_
#define CORE_DEFS_H_

#include <cstdio>
#include <cstdlib>

#ifdef __GNUC__
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#else
#define likely(x) (x)
#define unlikely(x) (x)
#endif

// @ASSERT will print the code line and file where the assert is false, say,
// @truth is false. After printing the code infomation, it will halt the
// program.
#define ASSERT(truth) \
    if (!(truth)) { \
      printf("\x1b[1;31mASSERT\x1b[0m, LINE:%d, FILE:%s\n", \
             __LINE__, __FILE__); \
      exit(EXIT_FAILURE); \
    } else

// Similar to @ASSERT, but @ASSERT_INFO will print another line called INFO.
// The output contents are specified by @info.
#define ASSERT_INFO(truth, info) \
    if (!(truth)) { \
      printf("\x1b[1;31mASSERT\x1b[0m, LINE:%d, FILE:%s\n", \
             __LINE__, __FILE__); \
      printf("\x1b[1;32mINFO\x1b[0m: %s\n", info); \
      exit(EXIT_FAILURE); \
    } else

// @ERROR will print @msg and then according to the truth value of @to_exit,
// it will halt the program or not. To be specific, when @to_exit is true,
// it will halt the program, otherwise, not.
#define ERROR(msg, to_exit) \
    if (true) { \
      printf("\x1b[1;31mERROR\x1b[0m: %s\n", msg); \
      if (to_exit) { \
        exit(EXIT_FAILURE); \
      } \
    } else

#endif
