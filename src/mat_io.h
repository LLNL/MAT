#ifndef MAT_IO_H_
#define MAT_IO_H_

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int mat_io_open(const char* path, int flags, ...);
ssize_t mat_io_write(int fd, const char* buf, size_t size);
ssize_t mat_io_read(int fd, void* buf, size_t size);


size_t mat_io_fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
int mat_io_fclose(FILE *stream);
FILE* mat_io_fopen(const char* path, const char* mode);

int mat_io_mkdir(const char *pathname, mode_t mode);

#endif
