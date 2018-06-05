#ifndef MAT_CONFIG_H_
#define MAT_CONFIG_H_

#include <string>

using namespace std;

/* #ifdef __cplusplus  */
/* extern "C" { */
/* #endif */

#define MAT_ENV_NAME_MOUNT_DIR      "PACFS_MOUNT_DIR"
#define MAT_ENV_NAME_CACHE_DIR      "PACFS_CACHE_DIR"
#define MAT_ENV_NAME_CHUNK_SIZE     "PACFS_CHUNK_SIZE"
#define MAT_ENV_NAME_PARTITION_METHOD "PACFS_PARTITION_METHOD"
#define MAT_ENV_VAL_PARTITION_METHOD_DEFAULT (0)
#define MAT_ENV_VAL_PARTITION_METHOD_STRIDE  (1)
#define MAT_ENV_VAL_PARTITION_METHOD_CUSTOM  (2)
#define MAT_ENV_NAME_PARTITION_SIZE  "PACFS_PARTITION_SIZE"

typedef struct {
  char*  mount_dir = ".";
  char*  cache_dir = "/tmp";
  size_t chunk_size = 1 * 1024 * 1024;
  int    partition_method = MAT_ENV_VAL_PARTITION_METHOD_DEFAULT;
  int    partition_size = 1;
} mat_config_t;

extern mat_config_t mat_config;

void mat_config_init(int *argc, char ***argv);

#endif
