#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <utility>

#include "FanzaiIPCClient.h"

FanzaiIPClient::FanzaiIPCClient(string clientName, string serviceName,
                                pid_t clientPid) {
  this->clientName = clientName;
  this->serviceName = serviceName;
  this->clientPid = clientPid;

  if (FanzaiIPC.insertProcessToMap(serviceName, pid,
                                   CLIENT_MAP_FILE_LOCATION) == -1) {
    throw "Same service name error\n";
  }

  this->servicePid =
      FanzaiIPC.getPidByName(serviceName, SERVICE_MAP_FILE_LOCATION);
}

int FanzaiIPClient::createShmemFd(char* name, int size) {
  int fd = shm_open(name, O_CREAT | O_RDWR | O_EXCL, 0777);

  if (fd < 0) {
    fd = shm_open(name, O_RDWR, 0777);

    ftruncate(fd, size);
  }

  return fd;
}

char* FanzaiIPClient::createShmemBuf(int length, int fd) {
  return (char*)mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
}

int FanzaiIPCClient::sendMessage(IPCMetadata* metadata,
                                 ClientSignalHandler handler) {
  // TODO: add callback
  union sigval sv;
  sv.sival_ptr = (void*)metadata;
  sigqueue(this->servicePid, FANZAI_SIGNAL, sv);
}