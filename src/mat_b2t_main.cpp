#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "mat.h"
#include "mat_util.h"
#include "mat_io.h"


// ip = int(vec[0])
//   access_address = int(vec[1])
//   block_address  = int(vec[3])
//   is_read        = int(vec[4])
//   object_size    = int(vec[6])
//   tid            = int(vec[7])

  
static void mat_b2t_print_mem_trace(size_t id, int tid, mat_trace_mem_t *mtrace)
{
  int is_read;
  size_t offset;

  is_read = (mtrace->type == MAT_TRACE_LOAD)? 1:0;
  offset = (unsigned long)mtrace->addr - (unsigned long)mtrace->head_addr;
  printf("%lu %lu %lu %lu %d %lu %lu %d\n",
	 id,
	 (unsigned long)mtrace->addr,
	 offset,
	 (unsigned long)mtrace->head_addr,
	 is_read,
	 mtrace->size,
	 mtrace->alloc_size,
	 tid);
}

static void mat_b2t_print(const char* trace_path)
{
  mat_trace_t mtrace;
  FILE *fd;
  struct stat st;
  size_t file_size, read_size = 0;
  int control, type;

  stat(trace_path, &st);
  file_size = st.st_size;

  fd = mat_io_fopen(trace_path, "r");
  while(mat_io_fread(&mtrace, sizeof(mat_trace_t), 1, fd)) {
    read_size += sizeof(mat_trace_t);
    switch(mtrace.control) {
    case MAT_TRACE:
      mat_b2t_print_mem_trace(mtrace.id, mtrace.tid, &mtrace.mem);
      break;
    case MAT_LOOP:
      break;
    default:
      MAT_ERR("No such control: %d", mtrace.control);
    }
    fprintf(stderr, "Progress = %lu \%\r", read_size * 100 / file_size);
  }
  mat_io_fclose(fd);
  return;
}

int main(int argc, char** argv)
{
  if (argc <= 1) {
    fprintf(stderr, "%s <input binary>\n", argv[0]);
    exit(0);
  }

  mat_b2t_print(argv[1]);

  return 0;
}
