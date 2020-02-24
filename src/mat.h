// Copyright 2019 Lawrence Livermore National Security, LLC
// See the top-level LICENSE file for details.
//
// SPDX-License-Identifier: Apache-2.0

#ifndef MAT_RR_H
#define MAT_RR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#ifdef __cplusplus
extern "C" {
#endif

#define xstr(s) str(s)
#define str(s) #s

#define MAT_CONTROL mat_control
#define MAT_CONTROL_STR xstr(MAT_CONTROL)
  //void MAT_CONTROL(int control, int file_id, int loc_id, int type, void *addr, size_t size);

#define MAT_TRACE_FILE_DIR    "."
#define MAT_TRACE_DIR_FORMAT    "%s/mat-%u"  
#define MAT_TRACE_FILE_FORMAT "trace-%d.mat"

#define MAT_INIT (0)
#define MAT_FIN  (1)

#define MAT_TRACE (10)  
#define MAT_TRACE_LOAD  (100)
#define MAT_TRACE_STORE (101)

#define MAT_LOOP (20)
#define MAT_LOOP_PREHEADER    (200)
#define MAT_LOOP_HEADER_BEGIN (201)
#define MAT_LOOP_BODY_BEGIN   (202)
#define MAT_LOOP_BODY_END     (203)
#define MAT_LOOP_LATCH_BEGIN  MAT_LOOP_BODY_END
#define MAT_LOOP_EXIT         (204)

#define MAT_BB (30)

#define MAT_DEBUG_PRINT (90)

typedef struct {
  int type;
  void *head_addr;
  size_t alloc_size;
  void *addr;
  size_t size;
  int num_insts;
} mat_trace_mem_t;

typedef struct {
  int type;
  int file_id;
  int loop_id;
} mat_trace_loop_t;

typedef struct {
  size_t size;
  int rest_num_insts;
} mat_trace_bb_t;

typedef struct {
  int control;
  size_t id;
  int tid;
  union {
    mat_trace_mem_t mem;
    mat_trace_loop_t loop;
    mat_trace_bb_t bb;
  };
} mat_trace_t;

void mat_enable();
void mat_disable();

#ifdef __cplusplus
}
#endif


#endif
