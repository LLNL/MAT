// Copyright 2019 Lawrence Livermore National Security, LLC
// See the top-level LICENSE file for details.
//
// SPDX-License-Identifier: Apache-2.0

#ifndef MAT_IO_H_
#define MAT_IO_H_

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int mat_io_open(const char* path, int flags, ...);
ssize_t mat_io_write(int fd, const char* buf, size_t size);
ssize_t mat_io_read(int fd, void* buf, size_t size);
int mat_io_close(int fd);

FILE* mat_io_fopen(const char* path, const char* mode);
size_t mat_io_fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t mat_io_fread(void* buf, size_t size, size_t nmemb, FILE *stream);
int mat_io_fclose(FILE *stream);


int mat_io_mkdir(const char *pathname, mode_t mode);


#endif
