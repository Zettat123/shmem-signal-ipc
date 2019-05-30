#include <signal.h>
#include <stdio.h>

// #include "FanzaiIPC.h"
#include "FanzaiIPCService.h"

FanzaiIPCService::FanzaiIPCService(string serviceName, pid_t servicePid) {
  this->serviceName = serviceName;
  this->servicePid = servicePid;
  this->serviceSignalHandler = NULL;

  if (FanzaiIPC::insertProcessToMap(serviceName, servicePid,
                                    SERVICE_MAP_FILE_LOCATION) == -1) {
    throw "Same service name error\n";
  }
}

void FanzaiIPCService::setRawHandler(RawSigactionHandler rsh) {
  this->rawHandler = rsh;
}

void FanzaiIPCService::wrapServiceSignalHandler(int signum, siginfo_t* info,
                                                void* context) {
  if (signum == FANZAI_SIGNAL) {
    // IPCMetadata* metadata = (IPCMetadata*)(info->si_value.sival_ptr);
    printf("1\n");
    printf("%d\n", info->si_value.sival_int);
    // printf("%d\n", metadata->type);
    // this->shmemFd =
    //     FanzaiIPC::createShmemFd(metadata->clientName, metadata->bufferSize);
    // this->shmemBuf =
    //     FanzaiIPC::createShmemBuf(metadata->bufferSize, this->shmemFd);

    // this->serviceSignalHandler(this->shmemBuf, metadata->bufferSize);
  }
}

int FanzaiIPCService::updateHandler(ServiceSignalHandler newHandler) {
  this->serviceSignalHandler = newHandler;

  struct sigaction sa;
  sa.sa_sigaction = this->rawHandler;
  sa.sa_flags = SA_SIGINFO;
  sigaction(FANZAI_SIGNAL, &sa, NULL);

  return 0;
}

FanzaiIPCService::~FanzaiIPCService() {
  FanzaiIPC::removeProcessFromMap(this->serviceName, SERVICE_MAP_FILE_LOCATION);
  printf("Service %s has been removed.\n", this->serviceName.data());
}
