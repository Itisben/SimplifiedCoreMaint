#pragma once
#include <omp.h>
#include <vector>
#include <set>
#include <queue>
#include <math.h>       /* pow */
#include <climits>
#include <atomic>
#include <assert.h>
#include <stdio.h>

using namespace std;

//atomic operations
#define atomic_read(v)      (*(volatile typeof(*v) *)(v))
#define atomic_write(v,a)   (*(volatile typeof(*v) *)(v) = (a))

#define fetch_or(a, b)      __sync_fetch_and_or(a,b)
#define cas(a, b, c)        __sync_bool_compare_and_swap(a,b,c)
#define atomic_add(a, b)        __sync_fetch_and_add(a, b);
#define atomic_sub(a, b)        __sync_fectch_and_sub(a, b);
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#if 1
//assert 
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
#else

#define ASSERT(truth) 
#define ASSERT_INFO(truth, info)
#define ERROR(msg, to_exit) 

#endif








/*this stack is more efficient than the STL stack
* push to tail and pop the head
* ??? this may out of the bound */
// class QUEUE {
// private:
//     std::vector<int> array_;
//     int head_; 
//     int tail_; // append position.
// public:
//     QUEUE(std::size_t size) { 
//         head_ = 0; tail_ = 0;
//         reserve(size);
//     }

//     QUEUE() { 
//         head_ = 0; tail_ = 0;
//         //reserve(size);
//     }

//     ~QUEUE() {}
//     void reserve(std::size_t size) {array_.reserve(size);}
//     inline bool empty() { return head_ >= tail_;}
//     inline void clear() { head_ = 0; tail_ = 0; array_.clear();}
//     inline void push(int v) { array_.push_back(v); tail_++; }
//     inline void pop() { head_++; }
//     inline void poptail(){array_.pop_back(); tail_--;}
//     inline void remove() {}
//     inline int top() { return array_[head_]; }
//     inline int size() {return tail_ - head_;}
// };


