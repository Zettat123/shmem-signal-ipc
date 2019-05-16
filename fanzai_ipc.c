#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

int create_shm_fd(char* name, int size) {
  int fd = shm_open(name, O_CREAT | O_RDWR | O_EXCL, 0777);

  if (c2sfd < 0) {
    fd = shm_open(name, O_RDWR, 0777);

    ftruncate(fd, size);
  }

  return fd;
}

void* create_shm_buf(int length, int fd) {
  return mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
}