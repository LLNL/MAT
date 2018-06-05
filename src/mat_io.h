#ifndef MAT_IO_H_
#define MAT_IO_H_

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

ssize_t mat_io_write(int fd, const char* buf, size_t size);
ssize_t mat_io_read(int fd, void* buf, size_t size);



#endif
