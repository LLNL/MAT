#include <stdlib.h>
#include <stdio.h>

#ifdef USE_MAT
#include <signal.h>
#define BEGIN					\
  do {						\
    raise(SIGUSR1);				\
  } while(0)

#define END					\
  do {						\
    raise(SIGUSR2);				\
  } while(0)
#elif USE_MAT_LLVM
#include <mat.h>
#define BEGIN					\
  do {						\
    mat_enable();				\
  } while(0)

#define END					\
  do {						\
    mat_disable();				\
  } while(0)

#else
#define BEGIN
#define END
#endif

//#define N (10 * 1000 * 1000)
#define N (1000)

int main(int argc, char** argv)
{
  BEGIN;
//   volatile char a[N], b[N];
// #pragma omp parallel for
//   for (int i = 0; i < N; i++) {
//     a[i] = b[i];
//   }
  
//   volatile int c[N], d[N];
// #pragma omp parallel for
//   for (int i = 0; i < N; i++) {
//     c[i] = d[i];
//   }

//   volatile double e[N], f[N];
// #pragma omp parallel for
//   for (int i = 0; i < N; i++) {
//     e[i] = f[i];
//   }

  volatile double *g, *h;
  g = (double*)malloc(sizeof(double) * N);
  h = (double*)malloc(sizeof(double) * N);
  #pragma omp parallel for
  for (int i = 0; i < N; i++) {
    g[i] = h[i];
  }
  free((void*)g);
  free((void*)h);
  
  fprintf(stderr, "done\n");
  END;  
  return 0;
}
