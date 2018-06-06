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

void MAT_CONTROL(int control, void* ptr, size_t size)
{
  if (mat_config.mode == MAT_ENV_NAME_MODE_DISABLE) return;

  switch(control) {
  case MAT_INIT:
    MAT_DBG("Control: %d", control);
    break;
  case MAT_TRACE: // 0
    MAT_DBG("Control: %d", control);
    break;
  case MAT_FIN:
    MAT_DBG("Control: %d", control);
    break;
  default:
    MAT_ERR("No such control: %d", control);

  }
  return;
}



