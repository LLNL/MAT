#ifndef MAT_RD_H_
#define MAT_RD_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mat.h>

#include <map>
#include <list>
#include <vector>

using namespace std;

typedef struct {
  size_t cache_line_size = 0;
} mat_rd_config;

typedef struct {
  size_t cache_line_id = -1;
  ssize_t reuse_distance = 0;
} mat_model_meta_t;

typedef struct {
  mat_trace_mem_t trace;
  mat_model_meta_t meta;
} mat_rd_mem_t;

typedef struct {
  mat_rd_config config;
  list<mat_rd_mem_t*> *access_list;
  map<ssize_t, vector<mat_rd_mem_t*>*> *rdist_map;
  size_t num_insts = 0;
} mat_rd_t;


mat_rd_t* mat_rd_create(mat_rd_config *config);
void mat_rd_input(mat_rd_t *rd, const char* trace_path);

void mat_rd_mem_access(mat_rd_t *rd,  mat_trace_mem_t *memt);
void mat_rd_loop(mat_rd_t *rd,  mat_trace_loop_t *loopt);

void mat_rd_print(mat_rd_t* rd);
void mat_rd_output(mat_rd_t* rd);
void mat_rd_free(mat_rd_t* rd);

#endif
