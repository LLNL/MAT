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
  id =  (size_t)addr / (size_t)rd->cache_line_size;
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
    if (m->cache_line_id == mem->cache_line_id) {
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

static void mat_rd_add_stat(mat_rd_t *rd, mat_rd_mem_t *mem, ssize_t rdistance)
{
  vector<mat_rd_mem_t*> *mem_vec;
  if (rd->rdist_map->find(rdistance) == rd->rdist_map->end()) {
    rd->rdist_map->insert(make_pair(rdistance, new vector<mat_rd_mem_t*>()));
  }
  mem_vec = rd->rdist_map->at(rdistance);
  mem_vec->push_back(mem);
  return;
}

mat_rd_t* mat_rd_create(size_t cache_line_size)
{
  mat_rd_t *rd = NULL;
  rd = (mat_rd_t*) malloc(sizeof(mat_rd_t));
  rd->cache_line_size = cache_line_size;
  rd->access_list    = new list<mat_rd_mem_t*>();
  rd->rdist_map = new map<ssize_t, vector<mat_rd_mem_t*>*>();
  return rd;
}

void mat_rd_mem_access(mat_rd_t *rd,  mat_trace_mem_t *memt)
{
  mat_rd_mem_t *m;
  ssize_t reuse_distance;

  /* Init memry access */
  m = (mat_rd_mem_t*)malloc(sizeof(mat_rd_mem_t));
  m->trace = *memt;

  /* Cache line ID */
  m->cache_line_id = mat_rd_get_cache_line_id(rd, m->trace.addr);

  /* Compute reuse distance*/
  reuse_distance   = mat_rd_compute_reuse_distance(rd, m);
  mat_rd_add_stat(rd, m, reuse_distance);
  
  //  MAT_DBG("addr: %lu size: %lu: dist: %d", m->trace.addr, m->trace.size, reuse_distance);
  return;
}

void mat_rd_loop(mat_rd_t *rd,  mat_trace_loop_t *loopt)
{
  
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
      break;
    default:
      MAT_ERR("No such control: %d", mtrace.control);
    }
    fprintf(stderr, "Progress = %d \%\r", read_size * 100 / file_size);
  }
  mat_io_fclose(fd);
  return;
}

void mat_rd_print(mat_rd_t* rd)
{
  map<ssize_t, vector<mat_rd_mem_t*>*>::iterator it, it_end;
  size_t total_bytes = 0;
  size_t total_access = 0;
  
  MAT_PRT("----------------------------------------------------------------");
  MAT_PRT("<reuse distance>\t<# of access>\t<access size(bytes)>");
  MAT_PRT("----------------------------------------------------------------");
  for (it = rd->rdist_map->begin(), it_end = rd->rdist_map->end();
       it != it_end;
       it++) {
    vector<mat_rd_mem_t*> *mem_vec;
    ssize_t dist = it->first;
    mem_vec = it->second;
    size_t length = mem_vec->size();
    size_t bytes = 0;
    for (size_t i = 0; i < length; i++) {
      mat_rd_mem_t *m;
      m = mem_vec->at(i);
      bytes += m->trace.size;
    }
    MAT_PRT("%15d\t%20lu\t%19lu", dist, length, bytes);
    //    MAT_PRT("%d, %lu, %lu", dist, length, bytes);
    total_access += length;
    total_bytes += bytes;
  }
  MAT_PRT("----------------------------------------------------------------");
  MAT_PRT("Total accesses: %lu", total_access);
  MAT_PRT("Total bytes: %lu", total_bytes);
  MAT_PRT("----------------------------------------------------------------");
}

void mat_rd_output(mat_rd_t* rd)
{}

void mat_rd_free(mat_rd_t* rd)
{}

