// Copyright 2019 Lawrence Livermore National Security, LLC
// See the top-level LICENSE file for details.
//
// SPDX-License-Identifier: Apache-2.0

#ifndef MAT_ALLOC_TREE_H_
#define MAT_ALLOC_TREE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void mat_alloc_tree_init();
void mat_alloc_tree_insert(void* addr, size_t size);
int mat_alloc_tree_lookup(void* addr, void** start_addr, size_t *alloc_size);
void mat_alloc_tree_remove(void* addr);



#endif
