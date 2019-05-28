#include <stdio.h>

#include "FanzaiIPCService.h"

FanzaiIPCService::FanzaiIPCService(string serviceName, pid_t servicePid) {
  this->serviceName = serviceName;
  this->servicePid = servicePid;
  this->serviceSignalHandler = NULL;

  if (FanzaiIPC.insertProcessToMap(serviceName, pid,
                                   SERVICE_MAP_FILE_LOCATION) == -1) {
    throw "Same service name error\n";
  }
}

void FanzaiIPCService::wrapServiceSignalHandler(int signum, siginfo_t* info,
                                                void* context) {
  if (signum == FANZAI_SIGNAL) {
    IPCMetadata* metadata = (IPCMetadata*)info->sival_ptr;
    this->shmemFd =
        FanzaiIPC::createShmemFd(metadata->clientName, metadata->bufferSize);
    this->shmemBuf =
        FanzaiIPC::createShmemBuf(metadata->bufferSize, this->shmemFd);

    this->serviceSignalHandler(this->shmemBuf, metadata->bufferSize);
  }
}

int FanzaiIPCService::updateHandler(ServiceSignalHandler newHandler) {
  this->serviceSignalHandler = newHandler;

  struct sigaction sa;
  sa.sa_sigaction = FanzaiIPCService::wrapServiceSignalHandler;
  sa.sa_flags = SA_SIGINFO;
  sigaction(FANZAI_SIGNAL, &sa, NULL);

  return 0;
}

FanzaiIPCService::~FanzaiIPCService() {
  FanzaiIPC.removeProcessFromMap(this->serviceName);
  printf("Service %s has been removed.\n", this->serviceName);
}