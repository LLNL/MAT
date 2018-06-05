/* ==========================ReMPI:LICENSE==========================================   
   Copyright (c) 2016, Lawrence Livermore National Security, LLC.                     
   Produced at the Lawrence Livermore National Laboratory.                            
                                                                                       
   Written by Kento Sato, kento@llnl.gov. LLNL-CODE-711357.                           
   All rights reserved.                                                               
                                                                                       
   This file is part of ReMPI. For details, see https://github.com/PRUNER/ReMPI       
   Please also see the LICENSE file for our notice and the LGPL.                      
                                                                                       
   This program is free software; you can redistribute it and/or modify it under the   
   terms of the GNU General Public License (as published by the Free Software         
   Foundation) version 2.1 dated February 1999.                                       
                                                                                       
   This program is distributed in the hope that it will be useful, but WITHOUT ANY    
   WARRANTY; without even the IMPLIED WARRANTY OF MERCHANTABILITY or                  
   FITNESS FOR A PARTICULAR PURPOSE. See the terms and conditions of the GNU          
   General Public License for more details.                                           
                                                                                       
   You should have received a copy of the GNU Lesser General Public License along     
   with this program; if not, write to the Free Software Foundation, Inc., 59 Temple   
   Place, Suite 330, Boston, MA 02111-1307 USA
   ============================ReMPI:LICENSE========================================= */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <iostream>

#include "mat_config.h"
#include "mat_util.h"

using namespace std;

mat_config_t mat_config;

void mat_config_init(int *argc, char ***argv)
{
  char* env;
  int env_int;

  if (NULL != (env = getenv(MAT_ENV_NAME_MOUNT_DIR))) {
    mat_config.mount_dir = env;
  }

  if (NULL != (env = getenv(MAT_ENV_NAME_CACHE_DIR))) {
    mat_config.cache_dir = env;
  }

  if (NULL != (env = getenv(MAT_ENV_NAME_CHUNK_SIZE))) {
    mat_config.chunk_size = (size_t)atoi(env);
  }

  if (NULL != (env = getenv(MAT_ENV_NAME_PARTITION_METHOD))) {
    mat_config.partition_method = atoi(env);
  }

  if (mat_config.partition_method == MAT_ENV_VAL_PARTITION_METHOD_DEFAULT) {
    if (NULL != (env = getenv(MAT_ENV_NAME_PARTITION_SIZE))) {
      mat_config.partition_size = atoi(env);
    }
  }

  return;
}


