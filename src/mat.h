#ifndef MAT_RR_H
#define MAT_RR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#ifdef __cplusplus
extern "C" {
#endif

#define xstr(s) str(s)
#define str(s) #s

#define MAT_CONTROL mat_control
#define MAT_CONTROL_STR xstr(MAT_CONTROL)
void MAT_CONTROL(int control, void* ptr, size_t size);

#define MAT_INIT (0)
#define MAT_FIN  (1)

#define MAT_TRACE (10)

#define MAT_DEBUG_PRINT (90)

#ifdef __cplusplus
}
#endif


#endif
