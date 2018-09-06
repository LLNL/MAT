#include <stdlib.h>
#include <stdio.h>

#include <mat_reuse_distance.h>
#include <mat_util.h>

int main(int argc, char** argv)
{
  mat_rd_t *rd;
  mat_rd_config_t config;
  if (argc <= 1) {
    fprintf(stderr, "%s <trace file path>\n", argv[0]);
    exit(0);
  }
  config.cache_line_size = 64;

  rd = mat_rd_create(&config);
  mat_rd_input(rd, argv[1]);
  mat_rd_print(rd);
  mat_rd_output(rd);
  mat_rd_free(rd);
  return 0;
}
