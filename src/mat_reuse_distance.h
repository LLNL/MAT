// Copyright 2019 Lawrence Livermore National Security, LLC
// See the top-level LICENSE file for details.
//
// SPDX-License-Identifier: Apache-2.0

#ifndef MAT_RD_H_
#define MAT_RD_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mat.h>

#include <map>
#include <unordered_map>
#include <list>
#include <vector>

using namespace std;

typedef struct {
  int num_levels = 0;
  int *latencies = NULL;
  size_t *sizes_bytes  = NULL;
} mat_model_config_mem_t;

typedef struct {
  double freqency_GHz = 0;
  size_t cache_line_size = 0;
  char* data_dependency_dir = NULL;
  mat_model_config_mem_t mem;
} mat_rd_config_t;

typedef struct {
  size_t cache_line_id = -1;
  ssize_t reuse_distance = 0;
  int num_insts = 0;
  int cycles = 0;
} mat_model_meta_t;

typedef struct {
  mat_trace_mem_t trace;
  mat_model_meta_t meta;
} mat_rd_mem_t;

typedef struct {
  size_t num_reads = 0;
  size_t num_writes = 0;
  size_t read_bytes = 0;
  size_t write_bytes = 0;
} mat_model_stat_rd_element_t;


typedef struct {
  size_t cycles = 0;
  size_t num_reads = 0;
  size_t num_writes = 0;
  size_t read_bytes = 0;
  size_t write_bytes = 0;
  size_t num_insts = 0;
  map<ssize_t, mat_model_stat_rd_element_t*> *rdist_stat_map;
} mat_model_stat_t;

typedef struct {
  mat_rd_config_t config; /* Configuration */
  mat_model_stat_t stat; /* Stat data printed out at the end */
  /*Other temporal data*/
  list<mat_rd_mem_t*> *access_list;
  int rest_num_insts = 0;
  list<mat_rd_mem_t*> *recent_access_list;
  /*This is not used */ map<ssize_t, vector<mat_rd_mem_t*>*> *rdist_map;
} mat_rd_t;


mat_rd_t* mat_rd_create(mat_rd_config_t *config);
void mat_rd_input(mat_rd_t *rd, const char* trace_path);

//void mat_rd_mem_access(mat_rd_t *rd,  mat_trace_mem_t *memt);
//void mat_rd_loop(mat_rd_t *rd,  mat_trace_loop_t *loopt);

void mat_rd_print(mat_rd_t* rd);
void mat_rd_output(mat_rd_t* rd);
void mat_rd_free(mat_rd_t* rd);

#endif
