#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <omp.h>
#include <mpi.h>
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


static void handle_trace(int control, int file_id, int loc_id, int type, void *addr, size_t size)
{
  int tid;
  FILE *fd;
  void* start_addr;
  size_t alloc_size;
    
  tid = get_tid();
  fd = get_fd(tid);
  //  fwrite(&file_id, sizeof(int), 1, fd);
  //  fwrite(&loc_id,  sizeof(int), 1, fd);
  //  fwrite(&type,    sizeof(int), 1 , fd);
  //  fwrite(&addr,    sizeof(void*), 1, fd);
  //  fwrite(&size,    sizeof(char), 1, fd);

  if (0 == mat_alloc_tree_lookup(addr, &start_addr, &alloc_size)) {
    MAT_DBG("Alloc: %p %lu", start_addr, alloc_size);
  }
  
  switch(type) {
  case MAT_TRACE_LOAD:
    //    MAT_DBG("LOAD: %d %d %p %lu", file_id, loc_id, addr, size);
    break;
  case MAT_TRACE_STORE:
    //    MAT_DBG("STOR: %d %d %p %lu", file_id, loc_id, addr, size);
    break;
  }
  return;
}


static void handle_loop(int control, int file_id, int loc_id, int type, void *addr, size_t id)
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

void MAT_CONTROL(int control, int file_id, int loc_id, int type, void *addr, size_t size)
{
  if (mat_config.mode == MAT_ENV_NAME_MODE_DISABLE) return;

  switch(control) {
  case MAT_INIT:
    init();
    break;
  case MAT_TRACE:
    handle_trace(control, file_id, loc_id, type, addr, size);
    break;
  case MAT_LOOP:
    handle_loop(control, file_id, loc_id, type, addr, size);
    break;
  case MAT_FIN:
    finalize();
    break;
  default:
    MAT_ERR("No such control: %d", control);

  }
  return;
}



