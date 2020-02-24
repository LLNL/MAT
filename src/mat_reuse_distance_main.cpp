// Copyright 2019 Lawrence Livermore National Security, LLC
// See the top-level LICENSE file for details.
//
// SPDX-License-Identifier: Apache-2.0

#include <stdlib.h>
#include <stdio.h>

#include <mat_reuse_distance.h>
#include <mat_util.h>

#define NUM_LEVELS (3)

int main(int argc, char** argv)
{
  mat_rd_t *rd;
  mat_rd_config_t config;
  int latencies[NUM_LEVELS]      = {4, 10, 40};
  size_t sizes_bytes[NUM_LEVELS] = {256, 256 * 1024, 30 * 1024 * 1024};
  

  if (argc <= 1) {
    fprintf(stderr, "%s <trace file path>\n", argv[0]);
    exit(0);
  }
  
  config.freqency_GHz = 2.4;
  config.cache_line_size = 64;

  config.mem.num_levels = NUM_LEVELS;
  config.mem.latencies = latencies;
  config.mem.sizes_bytes = sizes_bytes;
  
  rd = mat_rd_create(&config);
  mat_rd_input(rd, argv[1]);
  mat_rd_print(rd);
  mat_rd_output(rd);
  mat_rd_free(rd);
  return 0;
}
