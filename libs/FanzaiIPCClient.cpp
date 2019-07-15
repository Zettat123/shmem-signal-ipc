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

FanzaiIPCClient::FanzaiIPCClient(string serviceName, pid_t clientPid,
                                 int bufferLength) {
  this->serviceName = serviceName;
  this->clientPid = clientPid;
  this->bufferLength = bufferLength;

  this->shmemFileName = FANZAI_SHARED_MEMORY_FILE_NAME(clientPid);

  this->servicePid =
      FanzaiIPC::getPidByName(serviceName, SERVICE_MAP_FILE_LOCATION);

  this->shmemFd = FanzaiIPC::createShmemFd(this->shmemFileName, bufferLength);
  this->shmemBuf = FanzaiIPC::createShmemBuf(this->shmemFd, bufferLength);
  close(this->shmemFd);
}

void FanzaiIPCClient::establishConnection() {
  union sigval sv;
  sv.sival_int = this->bufferLength;

  char* fanzaiParams = this->shmemBuf;
  fanzaiParams[0] = 0;

  sigqueue(this->servicePid, FANZAI_SIGNAL, sv);

  // Wait for connection established.
  while (fanzaiParams[0] == 0) {
    usleep(1000 * 100);
  }
}

char* FanzaiIPCClient::getShmemBuf() {
  return this->shmemBuf + FANZAI_PARAMS_LENGTH;
}

void FanzaiIPCClient::wrappedServiceSignalHandler(int signum, siginfo_t* info,
                                                  void* context) {
  int type = info->si_value.sival_int;

  switch (type) {
    case FANZAI_COMMUNICATION:
      this->clientSignalHandler(this->shmemBuf + FANZAI_PARAMS_LENGTH);
      break;
  }
}

void FanzaiIPCClient::updateHandler(ClientSignalHandler newHandler) {
  this->clientSignalHandler = newHandler;

  struct sigaction sa;
  sa.sa_sigaction = this->rawHandler;
  sa.sa_flags = SA_SIGINFO;
  sigaction(FANZAI_SIGNAL, &sa, NULL);
}

void FanzaiIPCClient::setRawHandler(RawSigactionHandler handler) {
  this->rawHandler = handler;
}

void FanzaiIPCClient::signalService() {
  union sigval sv;
  sv.sival_int = FANZAI_COMMUNICATION;
  sigqueue(this->servicePid, FANZAI_SIGNAL, sv);
}

void FanzaiIPCClient::closeConnection() {
  this->removeShmem();

  union sigval sv;
  sv.sival_int = FANZAI_CLOSE_CONNECTION;
  sigqueue(this->servicePid, FANZAI_SIGNAL, sv);
}

int FanzaiIPCClient::removeShmem() {
  FanzaiIPC::munmapBuf((void*)this->shmemBuf, this->bufferLength);
  FanzaiIPC::unlinkShmem(this->shmemFileName);
  printf("Connection closed.\n");

  return 0;
}

FanzaiIPCClient::~FanzaiIPCClient() {
  printf("Client %d has been removed.\n", this->clientPid);
}
