// Copyright 2019 Lawrence Livermore National Security, LLC
// See the top-level LICENSE file for details.
//
// SPDX-License-Identifier: Apache-2.0

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#include "mat_util.h"

#define MAT_IO_RETRIES (10)

int mat_io_open(const char* path, int flags, ...)  
{

  int fd;

  /* if O_CREAT is set, it should have mode flags */
  int mode = 0;
  if (flags & O_CREAT) {
    va_list arg;
    va_start(arg, flags);
    mode = va_arg(arg, int);
    va_end(arg);
  }

  fd = open(path, flags, mode);

  if (fd < 0) {
    MAT_PRT("Open failed: path=%s: %s", path, strerror(errno));

    /* try again */
    int tries = 10;
    while (tries && fd < 0) {
      usleep(100 * 000);
      fd = open(path, flags, mode);

      tries--;
    }
    /* if we still don't have a valid file, consider it an error */
    if (fd < 0) {
      MAT_PRT("Open failed: path=%s: %s", path, strerror(errno));
    }
  }
  return fd;
}


FILE* mat_io_fopen(const char* path, const char* mode)
{
  FILE *fd;
  fd = fopen(path, mode);
  if (fd == NULL) {
    MAT_PRT("fopen retyring: path=%s: %s", path, strerror(errno));
    int tries = MAT_IO_RETRIES;
    while (tries && fd == NULL) {
      usleep(100 * 000);
      fd = fopen(path, mode);
      tries--;
    }
    if (fd == NULL) {
      MAT_PRT("fopen failed: path=%s: %s", path, strerror(errno));
    }
  }
  return fd;
}

int mat_io_close(int fd)
{
  if (close(fd) < 0) {
    MAT_ERR("close failed: fd=%d", fd);
  }
  return 0;
}


int mat_io_fclose(FILE *stream)
{
  if (fclose(stream) < 0) {
    MAT_ERR("fclose failed: fd=%p", stream);
  }
  return 0;
}
		

ssize_t  mat_io_write(int fd, const char* buf, size_t size)
{
  size_t n = 0;
  int retries = MAT_IO_RETRIES;
  while (n < size) {
    ssize_t rc = write(fd, (char*) buf + n, size - n);
    if (rc > 0) {
      n += rc;
    } else if (rc == 0) {
      /* something bad happened, print an error and abort */
      MAT_ERR("Error: write(%d, %p, %ld)",
	      fd, (char*) buf + n, size - n);
    } else { /* (rc < 0) */
      /* got an error, check whether it was serious */
      if (errno == EINTR || errno == EAGAIN) {
	continue;
      }
      /* something worth printing an error about */
      retries--;
      if (retries) {
	/* print an error and try again */
	MAT_DBG("write(%d, %p, %ld) errno=%d %s",
		fd, (char*) buf + n, size - n, errno, strerror(errno));
      } else {
	/* too many failed retries, give up */
	MAT_ERR("write(%d, %p, %ld) errno=%d %s",
		fd, (char*) buf + n, size - n, errno, strerror(errno));
      }
    }
  }
  return (ssize_t)n;
}

size_t mat_io_fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
  size_t n = 0;
  int retries = MAT_IO_RETRIES;
  while (n < nmemb) {
    size_t rc = fwrite((char*) ptr + size * n, size, nmemb - n, stream);
    if (rc > 0) {
      n += rc;
    } else if (rc == 0) {
      /* something bad happened, print an error and abort */
      MAT_ERR("fwrite failed");
    } else { /* (rc < 0) */
      /* something worth printing an error about */
      retries--;
      if (retries) {
	/* print an error and try again */
	MAT_DBG("fwrite retry");
      } else {
	/* too many failed retries, give up */
	MAT_ERR("fwrite failed");
      }
    }
  }
  return (ssize_t)n;
}



ssize_t mat_io_read(int fd, void* buf, size_t size)
{
  size_t n = 0;
  int retries = MAT_IO_RETRIES;
  while (n < size) {
    int rc = read(fd, (char*) buf + n, size - n);
    if (rc  > 0) {
      n += rc;
    } else if (rc == 0) {
      /* EOF */
      return (ssize_t)n;
    } else { /* (rc < 0) */
      /* got an error, check whether it was serious */
      if (errno == EINTR || errno == EAGAIN) {
	continue;
      }
      
      /* something worth printing an error about */
      retries--;
      if (retries) {
	/* print an error and try again */
	MAT_ERR("read(%d, %p, %ld) errno=%d %s",
		fd, (char*) buf + n, size - n, errno, strerror(errno));
      } else {
	/* too many failed retries, give up */
	MAT_ERR("read(%d, %p, %ld) errno=%d %s",
		fd, (char*) buf + n, size - n, errno, strerror(errno));
	exit(1);
      }
    }
  }
  return (ssize_t)n;
}

size_t mat_io_fread(void* buf, size_t size, size_t nmemb, FILE *stream)
{
  size_t n = 0;
  int retries = MAT_IO_RETRIES;
  while (n < nmemb) {
    size_t rc = fread((char*) buf + size * n, size, nmemb - n, stream);
    if (rc  > 0) {
      n += rc;
    } else if (rc == 0) {
      /* EOF */
      if (feof(stream)) return n;
      if (ferror(stream)) {
	MAT_ERR("fread failed: (%d, %p, $lu, $lu)",
		stream, (char*) buf + size * n, size, nmemb - n);
      }
    } else { /* (rc < 0) */
      /* got an error, check whether it was serious */
      if (errno == EINTR || errno == EAGAIN) {
	continue;
      }
      
      /* something worth printing an error about */
      retries--;
      if (retries) {
	/* print an error and try again */
	MAT_DBG("fread retry: (%d, %p, $lu, $lu)",
		stream, (char*) buf + size * n, size, nmemb - n);
      } else {
	/* too many failed retries, give up */
	MAT_ERR("fread failed: (%d, %p, $lu, $lu)",
		stream, (char*) buf + size * n, size, nmemb - n);
      }
    }
  }
  return (ssize_t)n;
}


int mat_io_mkdir(const char *pathname, mode_t mode)
{
  int ret;
  if ((ret = mkdir(pathname, mode)) < 0) {
    MAT_ERR("mkdir failed: path=%s: %s", pathname, strerror(errno));
  }
  return ret;
}
