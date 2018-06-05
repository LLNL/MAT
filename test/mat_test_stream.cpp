#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pacfs.h>
#include <mpi.h>

#define WSIZE (128 * 1000 * 1000)
#define RSIZE (32 * 1000 * 1000)


void write_test(char * path)
{
  int fd = open(path, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
  ssize_t size;
  char *buf = (char *)malloc(WSIZE);
  size = write(fd,buf, WSIZE);
  if (size < 0) {
    fprintf(stderr, "Write error\n");
    exit(1);
  }
  close(fd);
}


void read_test(char * path)
{
  int fd = open(path, O_RDONLY);
  ssize_t size;
  char *buf = (char *)malloc(RSIZE);
  while ((size = read(fd,buf, RSIZE)) != 0) {
    if (size < 0) {
      fprintf(stderr, "Read error\n");
      exit(1);
    } 
  }
  free(buf);
  close(fd);
}

void fwrite_test(char * path)
{
  FILE *fd = fopen(path, "w");
  ssize_t size;
  char *buf = (char *)malloc(WSIZE);
  size = fwrite(buf, WSIZE, 1, fd);
  if (size < 0) {
    fprintf(stderr, "fwrite error\n");
    exit(1);
  }
  fclose(fd);
}


void fread_test(char * path)
{
  FILE *fd = fopen(path, "r");
  size_t size;
  char *buf = (char *)malloc(RSIZE);
  while ((size = fread(buf, RSIZE, 1, fd)) != 0) {
    if (size < 0) {
      fprintf(stderr, "fread error\n");
      exit(1);
    }
  }
  fclose(fd);
}

void pac_test(int *argc, char ***argv, char* path)
{
  int fd;
  ssize_t size;
  char *buf;

  pac_init(argc, argv);
  fd = pac_open(path, O_RDONLY, S_IRUSR | S_IWUSR);
  buf = (char *)malloc(RSIZE);

  while ((size = pac_read(fd, buf, RSIZE)) != 0) {
    if (size < 0) {
      fprintf(stderr, "Pac read error\n");
      exit(1);
    }
  }
  free(buf);
  pac_close(fd);
}


int main(int argc, char **argv) {
  char* path;
  path = argv[1];
  //  MPI_Init(&argc, &argv);
  pac_test(&argc, &argv, path);
  // fprintf(stderr, "Writing ...\n");
  // write_test(path);
  //  fprintf(stderr, "Reading ...\n");
  //  read_test(path);
  // fprintf(stderr, "Fwriting ...\n");
  // fwrite_test(path);
  // fprintf(stderr, "Freading ...\n");
  // fread_test(path);
}

