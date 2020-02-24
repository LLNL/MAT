// Copyright 2019 Lawrence Livermore National Security, LLC
// See the top-level LICENSE file for details.
//
// SPDX-License-Identifier: Apache-2.0

#ifndef MAT_CONFIG_H_
#define MAT_CONFIG_H_

#include <string>

using namespace std;

/* #ifdef __cplusplus  */
/* extern "C" { */
/* #endif */

#define MAT_ENV_NAME_MODE "MAT_MODE"
#define MAT_ENV_NAME_MODE_ENABLE  (1)
#define MAT_ENV_NAME_MODE_DISABLE (0)


#define MAT_ENV_NAME_DIR    "MAT_DIR"
#define MAT_ENV_DIR_DEFAULT "."


typedef struct {
  int    mode = 0;
  char  *dir = NULL;
} mat_config_t;

extern mat_config_t mat_config;


void mat_config_init();

#endif
