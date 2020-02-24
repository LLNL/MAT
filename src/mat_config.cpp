// Copyright 2019 Lawrence Livermore National Security, LLC
// See the top-level LICENSE file for details.
//
// SPDX-License-Identifier: Apache-2.0

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

void mat_config_init()
{
  char* env;
  int env_int;

  if (NULL != (env = getenv(MAT_ENV_NAME_MODE))) {
    mat_config.mode = atoi(env);
  }

  if (NULL != (env = getenv(MAT_ENV_NAME_DIR))) {
    mat_config.dir = env;
  } else {
    mat_config.dir = MAT_ENV_DIR_DEFAULT;
  }

  MAT_DBG("MAT_DIR: %s", mat_config.dir);

  return;
}


