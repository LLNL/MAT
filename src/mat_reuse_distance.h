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
  mat_trace_mem_t trace;
  size_t cache_line_id;
} mat_rd_mem_t;

typedef struct {
  int cache_line_size = 64;
  list<mat_rd_mem_t*> *access_list;
  map<ssize_t, vector<mat_rd_mem_t*>*> *rdist_map;
} mat_rd_t;


mat_rd_t* mat_rd_create(size_t cacheline_size);
void mat_rd_input(mat_rd_t *rd, const char* trace_path);
void mat_rd_mem_access(mat_rd_t *rd,  mat_trace_mem_t *memt);
void mat_rd_loop(mat_rd_t *rd,  mat_trace_loop_t *loopt);
void mat_rd_print(mat_rd_t* rd);
void mat_rd_output(mat_rd_t* rd);
void mat_rd_free(mat_rd_t* rd);

#endif
