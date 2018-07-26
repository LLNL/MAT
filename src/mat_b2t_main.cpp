#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>


#include <mat.h>
#include <mat_util.h>
#include <mat_io.h>


static void mat_b2t_print_mem_trace(mat_trace_mem_t *mtrace)
{
  printf("%d %lu %lu\n", mtrace->type, (unsigned long)mtrace->addr, mtrace->size);
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
      mat_b2t_print_mem_trace(&mtrace.mem);
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
