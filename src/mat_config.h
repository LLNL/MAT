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

typedef struct {
  int    mode = 1;
} mat_config_t;

extern mat_config_t mat_config;

void mat_config_init(int *argc, char ***argv);

#endif
