#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include <mpi.h>

#include "mat_util.h"

#define MAT_IO_RETRIES (10)

ssize_t  mat_io_write(int fd, const char* buf, size_t size)
{
  ssize_t n = 0;
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
  return n;
}

ssize_t mat_io_read(int fd, void* buf, size_t size)
{
  ssize_t n = 0;
  int retries = MAT_IO_RETRIES;
  while (n < size)
    {
      int rc = read(fd, (char*) buf + n, size - n);
      if (rc  > 0) {
	n += rc;
      } else if (rc == 0) {
	/* EOF */
	return n;
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
  return n;
}
