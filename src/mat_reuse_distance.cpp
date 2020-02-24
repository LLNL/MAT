// Copyright 2019 Lawrence Livermore National Security, LLC
// See the top-level LICENSE file for details.
//
// SPDX-License-Identifier: Apache-2.0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <map>
#include <list>
#include <vector>

#include "mat_util.h"
#include "mat.h"
#include "mat_io.h"
#include "mat_reuse_distance.h"

static size_t mat_rd_get_cache_line_id(mat_rd_t *rd, void* addr)
{
  size_t id;
  id =  (size_t)addr / (size_t)rd->config.cache_line_size;
  return id;
}

static ssize_t mat_rd_compute_reuse_distance(mat_rd_t *rd, mat_rd_mem_t *mem)
{
  ssize_t rdist = 0, reuse_distance = -1;

  list<mat_rd_mem_t*>::iterator it, it_end;
  
  for (it = rd->access_list->begin(), it_end = rd->access_list->end();
       it != it_end;
       it++) {
    mat_rd_mem_t *m;
    m  = *it;
    if (m->meta.cache_line_id == mem->meta.cache_line_id) {
      rd->access_list->erase(it);
      reuse_distance  = rdist;
      break;
    } else {
      rdist++;
    }    
  }
  rd->access_list->push_front(mem);  
  return reuse_distance;
}

static void mat_rd_add_stat(mat_rd_t *rd, mat_rd_mem_t *mem)
{
  vector<mat_rd_mem_t*> *mem_vec;
  mat_model_stat_t *stat;
  mat_model_stat_rd_element_t *e;
    
  ssize_t rdistance = mem->meta.reuse_distance;

  /* Keep memory access in map for further batch analysis */
  if (rd->rdist_map->find(rdistance) == rd->rdist_map->end()) {
    rd->rdist_map->insert(make_pair(rdistance, new vector<mat_rd_mem_t*>()));
  }
  mem_vec = rd->rdist_map->at(rdistance);
  mem_vec->push_back(mem);

  /* Increment performance numbers */
  stat = &rd->stat;
  stat->cycles = mem->meta.cycles;
  if (stat->rdist_stat_map->find(rdistance) == stat->rdist_stat_map->end()) {
    e = (mat_model_stat_rd_element_t*)malloc(sizeof(mat_model_stat_rd_element_t));
    memset(e, 0, sizeof(mat_model_stat_rd_element_t));
    stat->rdist_stat_map->insert(make_pair(rdistance, e));
  }
  e = stat->rdist_stat_map->at(rdistance);
  if (mem->trace.type == MAT_TRACE_LOAD) {
    stat->num_reads++;
    stat->read_bytes += mem->trace.size;
    e->read_bytes += mem->trace.size;
    e->num_reads++;
  } else if (mem->trace.type == MAT_TRACE_STORE) {
    stat->num_writes++;
    stat->write_bytes += mem->trace.size;
    e->write_bytes += mem->trace.size;
    e->num_writes++;
  } else {
    MAT_ERR("No such access type: %d", mem->trace.type);
  }
  stat->num_insts += mem->meta.num_insts;


  
  return;
}

mat_rd_t* mat_rd_create(mat_rd_config_t *config)
{
  mat_rd_t *rd = NULL;
  mat_model_stat_t *stat;
  rd = (mat_rd_t*) malloc(sizeof(mat_rd_t));
  rd->config = *config;
  memset(&rd->stat, 0, sizeof(mat_model_stat_t));
  stat = &rd->stat;
  stat->rdist_stat_map = new map<ssize_t, mat_model_stat_rd_element_t*>();

  rd->access_list    = new list<mat_rd_mem_t*>();
  rd->recent_access_list    = new list<mat_rd_mem_t*>();
  rd->rdist_map = new map<ssize_t, vector<mat_rd_mem_t*>*>();
  return rd;
}

static int mat_model_get_memory_access_cycles(mat_rd_t *rd, mat_rd_mem_t *m)
{
  mat_rd_config_t *config;
  int level = 0;
  config = &rd->config;

  if (m->meta.reuse_distance != -1) {
    for (level = 0; level < config->mem.num_levels; level++) {
      if (m->meta.reuse_distance * config->cache_line_size < config->mem.sizes_bytes[level]) break;
    }
  } else {
    level = config->mem.num_levels;
  }
  return config->mem.latencies[level];
}

static int  mat_model_compute_cycles(mat_rd_t *rd, mat_rd_mem_t *m)
{
  mat_rd_config_t *config;
  size_t cycles;

  list<mat_rd_mem_t*>::iterator it, it_end;
  // for (it = rd->recent_access_list.begin(),
  //      it_end =)
  
  
  config = &rd->config;
  

  cycles = mat_model_get_memory_access_cycles(rd, m);
  //  config->mem.latencies[config->mem.num_levels];
  return 0;
}

static void mat_model_compute_meta(mat_rd_t *rd, mat_rd_mem_t *m)
{
  /* Cache line ID */
  m->meta.cache_line_id = mat_rd_get_cache_line_id(rd, m->trace.addr);
  /* Compute reuse distance*/
  m->meta.reuse_distance   = mat_rd_compute_reuse_distance(rd, m);
  /* # of instructions including this store or load since the last store or load */
  m->meta.num_insts = rd->rest_num_insts + m->trace.num_insts;
  rd->rest_num_insts = 0;
  /* # of cycles to complete this memory access */
  m->meta.cycles = mat_model_compute_cycles(rd, m);
  
}

static void mat_rd_mem_access(mat_rd_t *rd,  mat_trace_mem_t *memt)
{
  mat_rd_mem_t *m;
  ssize_t reuse_distance;

  /* Init memry access */
  m = (mat_rd_mem_t*)malloc(sizeof(mat_rd_mem_t));
  m->trace = *memt;

  mat_model_compute_meta(rd, m);

  /* Add and Compute statistics */
  mat_rd_add_stat(rd, m);
  
  //  MAT_DBG("addr: %lu size: %lu: dist: %d", m->trace.addr, m->trace.size, reuse_distance);
  return;
}

static void mat_rd_loop(mat_rd_t *rd,  mat_trace_loop_t *loopt)
{
  
}

static void mat_rd_bb(mat_rd_t *rd, mat_trace_bb_t *bb)
{
  rd->rest_num_insts = bb->rest_num_insts;
  return;
}

void mat_rd_input(mat_rd_t *rd, const char* trace_path)
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
      mat_rd_mem_access(rd, &mtrace.mem);
      break;
    case MAT_LOOP:
      mat_rd_loop(rd, &mtrace.loop);
    case MAT_BB:
      mat_rd_bb(rd, &mtrace.bb);
      break;
    default:
      MAT_ERR("No such control: %d", mtrace.control);
    }
    fprintf(stderr, "RD Progress = %d \%\r", read_size * 100 / file_size);
  }
  mat_io_fclose(fd);
  return;
}

void mat_rd_print(mat_rd_t* rd)
{
  mat_model_stat_t *stat;
  map<ssize_t, mat_model_stat_rd_element_t*>::iterator it, it_end;
  stat = &rd->stat;

#define MAT_PRT_LINE							\
  do {									\
    MAT_PRT("-------------------------------------------------------------------------------------------------------------------"); \
  } while(0);

  MAT_PRT_LINE;
  MAT_PRT("%15s%20s%20s%19s%19s",
	  "<reuse distance>",
	  "<# of read>", "<# of write>" ,
	  "<read(bytes)>", "<write(bytes)>");
  MAT_PRT_LINE;

  for (it = stat->rdist_stat_map->begin(), it_end = stat->rdist_stat_map->end();
       it != it_end;
       it++) {
    ssize_t rdist = it->first;
    mat_model_stat_rd_element_t *e = it->second;
    if (rdist == -1) {
      MAT_PRT("%15s%20lu%20lu%19lu%19lu", "(first-touch)", e->num_reads, e->num_writes, e->read_bytes, e->write_bytes);
    } else {
      MAT_PRT("%15d%20lu%20lu%19lu%19lu", rdist, e->num_reads, e->num_writes, e->read_bytes, e->write_bytes);
    }
  }
  
  MAT_PRT_LINE;
  MAT_PRT("%15s%20lu%20lu%19lu%19lu", "Total", stat->num_reads, stat->num_writes, stat->read_bytes, stat->write_bytes);
  MAT_PRT_LINE;
  MAT_PRT("# of instructions: %lu", rd->stat.num_insts);
  MAT_PRT_LINE;
}

void mat_rd_output(mat_rd_t* rd)
{}

void mat_rd_free(mat_rd_t* rd)
{}

