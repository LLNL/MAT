// Copyright 2019 Lawrence Livermore National Security, LLC
// See the top-level LICENSE file for details.
//
// SPDX-License-Identifier: Apache-2.0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <omp.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <linux/limits.h>

#include "mat.h"
#include "mat_io.h"
#include "mat_gotcha.h"
#include "mat_config.h"
#include "mat_util.h"
#include "mat_alloc_tree.h"

#define MAT_MAX_NUM_THREADS (256)
#define MAT_INST_ID(file_id, loc_id) ((file_id + 1) * (loc_id))

typedef struct {
  FILE *trace_fd = NULL;
  char trace_name[PATH_MAX];
} mat_tls_t;

char mat_dir_path[PATH_MAX];
mat_tls_t mat_tls[MAT_MAX_NUM_THREADS];

static int get_tid()
{
  return omp_get_thread_num();
}

static void init()
{
  pid_t pid;

  mat_config_init();
  mat_gotcha_init();
  
  pid = getpid();
  sprintf(mat_dir_path, MAT_TRACE_DIR_FORMAT, mat_config.dir, pid);
  mat_io_mkdir(mat_dir_path, 0700);
  return;
}

static void finalize()
{
  for (int i = 0; i < MAT_MAX_NUM_THREADS; i++) {
    mat_tls_t *tls;
    tls = &mat_tls[i];
    if (tls->trace_fd != NULL) {
      mat_io_fclose(tls->trace_fd);
    }
  }
  MAT_DBG("Finalize:");
  return;
}

static FILE* get_fd(int tid)
{
  mat_tls_t *tls;
  tls = &mat_tls[tid];
  if (tls->trace_fd == NULL) {
    sprintf(tls->trace_name, "%s/" MAT_TRACE_FILE_FORMAT, mat_dir_path, tid);
    tls->trace_fd = mat_io_fopen(tls->trace_name, "w+");
  }
  return tls->trace_fd;
}


static void handle_trace(int control, size_t id, int type, void *addr, size_t size, int num_insts)
{
  int tid;
  FILE *fd;
  void* start_addr  = 0;
  size_t alloc_size = 0;
  mat_trace_t mtrace;
    
  tid = get_tid();
  fd = get_fd(tid);

  mat_alloc_tree_lookup(addr, &start_addr, &alloc_size);

  mtrace.control  = control;
  mtrace.id       = id;
  mtrace.tid      = tid;
  mtrace.mem.type = type;
  mtrace.mem.head_addr = start_addr;
  mtrace.mem.alloc_size = alloc_size;
  mtrace.mem.addr = addr;
  mtrace.mem.size = size;
  mtrace.mem.num_insts = num_insts;
  fwrite(&mtrace, sizeof(mat_trace_t), 1, fd);
  
#if 0  
  if (0 == mat_alloc_tree_lookup(addr, &start_addr, &alloc_size)) {
    fwrite(&file_id, sizeof(int), 1, fd);
    fwrite(&loc_id,  sizeof(int), 1, fd);
    fwrite(&type,    sizeof(int), 1 , fd);
    fwrite(&addr,    sizeof(void*), 1, fd);
    fwrite(&size,    sizeof(char), 1, fd);
    fwrite(&tid,     sizeof(int), 1, fd);
    //    fwrite(&start_addr, sizeof(void*), 1 , fd);
    //    fwrite(&alloc_size, sizeof(size_t), 1 , fd);
    // MAT_PRT("%d %lu %d %d %lu %lu",
    // 	    type, addr, size, tid, start_addr, alloc_size);
    MAT_PRT("%d %d %d %lu %d %d %lu %lu",
	    file_id, loc_id, type, addr, size, tid, start_addr, alloc_size);
  } else {
    MAT_PRT("%d %d %d %lu %d %d",
	    file_id, loc_id, type, addr, size, tid);
  }
#endif

  return;
}


static void handle_loop(int control, size_t global_id, int type, void *addr, size_t id, int num_insts)
{
  switch(type) {
  case MAT_LOOP_PREHEADER:
    //    MAT_DBG("LOOP: %lu: == LOOP: BEGIN == ", id);
    break;
  case MAT_LOOP_HEADER_BEGIN:
    //    MAT_DBG("LOOP: %lu: -- LOOP: HEADER BEGIN --", id);
    break;
  case MAT_LOOP_BODY_BEGIN:
    //    MAT_DBG("LOOP: %lu: -- LOOP: BODY BEGIN --", id);
    break;
  case MAT_LOOP_BODY_END:
    //    MAT_DBG("LOOP: %lu: -- LOOP: BODY END --", id);
    break;
  case MAT_LOOP_EXIT:
    //    MAT_DBG("LOOP: %lu: == LOOP: EXIT ==", id);
    break;
  }
}


static void handle_bb(int control, size_t id, int type, void *addr, size_t size, int num_insts)
{
  int tid;
  FILE *fd;
  mat_trace_t mtrace;

  tid = get_tid();
  fd = get_fd(tid);
  
  mtrace.control  = control;
  mtrace.id       = id;
  mtrace.tid      = tid;
  mtrace.bb.size  = size;
  mtrace.bb.rest_num_insts = num_insts;
  fwrite(&mtrace, sizeof(mat_trace_t), 1, fd);
  return;
}

void mat_enable()
{
  mat_config.mode = MAT_ENV_NAME_MODE_ENABLE;
  MAT_PRT("Enabled");
}

void mat_disable()
{
  mat_config.mode = MAT_ENV_NAME_MODE_DISABLE;
  MAT_PRT("Disabled");
}

extern "C" void MAT_CONTROL(int control, size_t id, int type, void *addr, size_t size, int num_insts)
//void MAT_CONTROL(int control, int file_id, int loc_id, int type, void *addr, size_t size)
{
  switch(control) {
  case MAT_INIT:
    init();
    return;
  case MAT_FIN:
    finalize();
    //    MAT_TIMER(MAT_TIMER_PRINT, 0, NULL);
    return;
  }

  if (mat_config.mode == MAT_ENV_NAME_MODE_DISABLE) return;

  switch(control) {
  case MAT_TRACE:
    handle_trace(control, id, type, addr, size, num_insts);
    break;
  case MAT_LOOP:
    handle_loop(control, id, type, addr, size, num_insts);
    break;
  case MAT_BB:
    handle_bb(control, id, 0, NULL, size, num_insts);
    break;
  default:
    MAT_ERR("No such control: %d", control);
  }

  return;
}



