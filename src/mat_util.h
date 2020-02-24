// Copyright 2019 Lawrence Livermore National Security, LLC
// See the top-level LICENSE file for details.
//
// SPDX-License-Identifier: Apache-2.0

#ifndef MUTIL_H__
#define MUTIL_H__

#include <string>
#include <assert.h>
#include <stdlib.h>


#define MUTIL_FUNC_(prefix, func)   prefix ## _ ## func
#define MUTIL_FUNC(func)   MUTIL_FUNC_(mat_util, func)
#define MUTIL_PREFIX "MAT"
//#define MUTIL_MACRO_(prefix, name)   prefix ## _ ## name
//#define MUTIL_MACRO(name) MUTIL_MACRO_(MUTIL_PREFIX, name)

extern int mutil_my_rank;
extern char mutil_hostname[256];

#define MUTIL_PRT MAT_PRT
#define MAT_PRT(fmt, ...)			\
  do { \
  fprintf(stderr, fmt "\n", \
	  ## __VA_ARGS__);  \
  } while(0)

#define MUTIL_DBG MAT_DBG
#define MAT_DBG(fmt, ...)		\
  do { \
  fprintf(stderr, MUTIL_PREFIX ":DBG: " fmt " (%s:%d)\n", \
	   ## __VA_ARGS__, __FILE__, __LINE__);		  \
  } while(0)


#define MUTIL_ERR MAT_ERR
#define MAT_ERR(fmt, ...)		\
  do { \
  fprintf(stderr, MUTIL_PREFIX ":ERR: " fmt " (%s:%d)\n", \
	  ## __VA_ARGS__, __FILE__, __LINE__);	      \
  exit(1); \
  } while(0)

#define MUTIL_ASSERT MAT_ASSERT
#define MAT_ASSERT(b)			\
  do {                \
    assert(b);			\
  } while(0)          \


/*if __VA_ARGS__ is empty, the previous comma can be removed by "##" statement*/
/* #define MUTIL_DBGI(i, dbg_fmt, ...)		\ */
/*   do { \ */
/*   MUTIL_FUNC(dbgi)(i,				\ */
/* 	     " "				\ */
/* 	     dbg_fmt				\ */
/* 	     " (%s:%d)",			\ */
/* 	     ## __VA_ARGS__,			\ */
/*             __FILE__, __LINE__); \ */
/*   } while(0) */

#define ENABLE_TIMER
#define MUTIL_TIMER_START     (0)
#define MUTIL_TIMER_STOP      (1)
#define MUTIL_TIMER_GET_TIME  (2)
#define MUTIL_TIMER_PRINT     (3)
#ifdef ENABLE_TIMER
#define MUTIL_TIMER(mode, timerIndex, time) \
  do { \
    MUTIL_FUNC(timer)(mode, timerIndex, time);	\
  } while(0)
#else
#define MUTIL_TIMER(mode, timerIndex, time)
#endif

#define MAT_TIMER_START     (0)
#define MAT_TIMER_STOP      (1)
#define MAT_TIMER_GET_TIME  (2)
#define MAT_TIMER_PRINT     (3)
#define MAT_TIMER MUTIL_TIMER

using namespace std;
#ifdef __cplusplus
extern "C" {
#endif

void MUTIL_FUNC(set_configuration)(int *argc, char ***argv);
void MUTIL_FUNC(assert)(int b);
void MUTIL_FUNC(init)(int r);
void MUTIL_FUNC(err)(const char* fmt, ...);
void MUTIL_FUNC(erri)(int r, const char* fmt, ...);
void MUTIL_FUNC(alert)(const char* fmt, ...);
void MUTIL_FUNC(dbg_log_print)();
void MUTIL_FUNC(dbg)(const char* fmt, ...);
void MUTIL_FUNC(dbgi)(int r, const char* fmt, ...);


  //string MUTIL_FUNC(btrace_string)();
void MUTIL_FUNC(btrace)();
int MUTIL_FUNC(btrace_get)(char*** addresses, int len);
size_t MUTIL_FUNC(btrace_hash)();
void MUTIL_FUNC(btrace_line)();
void MUTIL_FUNC(exit)(int no);
void* MUTIL_FUNC(malloc)(size_t size);
void* MUTIL_FUNC(realloc)(void *ptr, size_t size);
void MUTIL_FUNC(free)(void* addr);
double MUTIL_FUNC(get_time)(void);
void MUTIL_FUNC(timer)(int mode, int timerIndex, double *time);
void MUTIL_FUNC(dbg_sleep_sec)(int sec);
void MUTIL_FUNC(sleep_sec)(int sec);
void MUTIL_FUNC(dbg_sleep_usec)(int sec);
void MUTIL_FUNC(sleep_usec)(int sec);
unsigned int MUTIL_FUNC(hash)(unsigned int original_val, unsigned int new_val);
unsigned int MUTIL_FUNC(hash_str)(const char* c, unsigned int length);
int MUTIL_FUNC(init_rand)(int seed);
int MUTIL_FUNC(init_ndrand)();
int MUTIL_FUNC(get_rand)(int max);
int MUTIL_FUNC(str_starts_with)(const char* base, const char* str);

size_t Backtrace(void **frames, size_t size);
void PrintBacktrace(void);

#ifdef __cplusplus
}
#endif


#endif
