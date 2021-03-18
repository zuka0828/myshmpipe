#include <stdio.h>
#include <fcntl.h> /* O_* */
#include <sys/mman.h> /* shm_open() */
#include <sys/stat.h> /* fstat() */
#include <unistd.h> /* ftruncate() */

#define SHM_NAME "/myshmpipe"

int main(int argc, char **argv)
{
  char path[32], tmp[32];
  int flags, fd;
  mode_t perms = S_IRUSR | S_IWUSR | S_IRGRP;
  off_t len = 64000000; /* 64MB */
  struct stat stat;
  void *shm_addr;

  /* Parse the argument (memory size) */
  if (argc > 1)
    len = (off_t)atoi(argv[1]);

  snprintf(path, sizeof(path), SHM_NAME);

  flags = O_RDWR | O_CREAT | O_TRUNC | O_EXCL;
  if ((fd = shm_open(path, flags, perms)) < 0) {
    perror("shm_open()");
    return -1;
  }

  if (ftruncate(fd, len) < 0) {
    perror("ftruncate()");
    goto err_unlink;
  }
  if (fstat(fd, &stat) < 0) {
    perror("fstat()");
    goto err_unlink;
  }
  printf("mmap: size=%ld\n", stat.st_size);
  /* mmap(addr, length, prot, flags, fd, offset) */
  if ((shm_addr = mmap(NULL, stat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) < 0) {
    perror("mmap()");
    goto err_unlink;
  }

  printf("Please enter to continue: ");
  fgets(tmp, sizeof(tmp), stdin);

  if (munmap(shm_addr, stat.st_size) < 0) {
    perror("munmap()");
    goto err_unlink;
  }
  if (shm_unlink(path) < 0) {
    perror("shm_unlink()");
    return -1;
  }
  return 0;

 err_munmap:
  munmap(shm_addr, stat.st_size);
 err_unlink:
  shm_unlink(path);
  return -1;
}
