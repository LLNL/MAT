// Copyright 2019 Lawrence Livermore National Security, LLC
// See the top-level LICENSE file for details.
//
// SPDX-License-Identifier: Apache-2.0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <map>

#include "mat_util.h"

map<void*, size_t> *addr2size_map;

void mat_alloc_tree_init()
{
  addr2size_map = new map<void*, size_t>();
  return;
}
			  
void mat_alloc_tree_insert(void* addr, size_t size)
{
  addr2size_map->insert(make_pair(addr, size));
  return;
}

int mat_alloc_tree_lookup(void* addr, void** start_addr, size_t *alloc_size)
{
  void *saddr;
  size_t asize;
  map<void*, size_t>::iterator alloc_key = addr2size_map->lower_bound(addr);
  if (alloc_key != addr2size_map->end()) {
    saddr  = alloc_key->first;
    asize  = alloc_key->second;
    if (addr == saddr) {
      *start_addr = saddr;
      *alloc_size = asize;
      return 0;
    }
  }

  if (alloc_key == addr2size_map->begin()) return -1;

  //  MAT_DBG("first: %p %p %p", saddr, addr, (char*)saddr + asize);
  
  alloc_key--;
  saddr  = alloc_key->first;
  asize  = alloc_key->second;

  //  MAT_DBG("second: %p %p %p", saddr, addr, (char*)saddr + asize);
  if (saddr <= addr && addr < (char*)saddr + asize) {
    *start_addr = saddr;
    *alloc_size = asize;
  } else {
    return -1;
  }
  return 0;
}

void mat_alloc_tree_remove(void* addr)
{
  addr2size_map->erase(addr);
  return;
}



