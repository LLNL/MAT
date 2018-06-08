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
#include "mat_config.h"
#include "mat_util.h"

static void init()
{
  MAT_DBG("Init:");
}

static void finalize()
{
  MAT_DBG("Finalize:");
}

static int get_tid()
{
  return omp_get_thread_num();
}


static void handle_trace(int control, int file_id, int loc_id, int type, void *addr, size_t size)
{
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



