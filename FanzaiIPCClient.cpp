#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <utility>

#include "FanzaiIPCClient.h"

FanzaiIPClient::FanzaiIPCClient(string clientName, string serviceName) {
  this->clientName = clientName;
  this->serviceName = serviceName;
}

int FanzaiIPClient::create_shm_fd(char* name, int size) {
  int fd = shm_open(name, O_CREAT | O_RDWR | O_EXCL, 0777);

  if (fd < 0) {
    fd = shm_open(name, O_RDWR, 0777);

    ftruncate(fd, size);
  }

  return fd;
}

char* FanzaiIPClient::create_shm_buf(int length, int fd) {
  return (char*)mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
}