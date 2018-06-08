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
void MAT_CONTROL(int control, int file_id, int loc_id, int type, void *addr, size_t size);

#define MAT_INIT (0)
#define MAT_FIN  (1)

#define MAT_TRACE (10)  
#define MAT_TRACE_LOAD  (100)
#define MAT_TRACE_STORE (101)

#define MAT_LOOP (20)
#define MAT_LOOP_PREHEADER    (200)
#define MAT_LOOP_HEADER_BEGIN (201)
#define MAT_LOOP_BODY_BEGIN   (202)
#define MAT_LOOP_BODY_END     (203)
#define MAT_LOOP_LATCH_BEGIN  MAT_LOOP_BODY_END
#define MAT_LOOP_EXIT         (204)

#define MAT_DEBUG_PRINT (90)

#ifdef __cplusplus
}
#endif


#endif
