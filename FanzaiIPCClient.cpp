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

FanzaiIPCClient::FanzaiIPCClient(string clientName, string serviceName,
                                 pid_t clientPid, int bufferSize) {
  this->clientName = clientName;
  this->serviceName = serviceName;
  this->clientPid = clientPid;
  this->bufferSize = bufferSize;

  this->servicePid =
      FanzaiIPC::getPidByName(serviceName, SERVICE_MAP_FILE_LOCATION);

  this->shmemFd = FanzaiIPC::createShmemFd(to_string(clientPid), bufferSize);
  this->shmemBuf = FanzaiIPC::createShmemBuf(this->shmemFd, bufferSize);
  close(this->shmemFd);
}

void* FanzaiIPCClient::getShmemBuf() { return this->shmemBuf; }

void FanzaiIPCClient::wrapServiceSignalHandler(int signum, siginfo_t* info,
                                               void* context) {
  this->clientSignalHandler(this->shmemBuf);
}

int FanzaiIPCClient::updateHandler(ClientSignalHandler newHandler) {
  this->clientSignalHandler = newHandler;

  struct sigaction sa;
  sa.sa_sigaction = this->rawHandler;
  sa.sa_flags = SA_SIGINFO;
  sigaction(FANZAI_SIGNAL, &sa, NULL);

  return 0;
}

void FanzaiIPCClient::setRawHandler(RawSigactionHandler handler) {
  this->rawHandler = handler;
}

int FanzaiIPCClient::sendMessage() {
  union sigval sv;
  sv.sival_int = this->bufferSize;
  sigqueue(this->servicePid, FANZAI_SIGNAL, sv);
}

FanzaiIPCClient::~FanzaiIPCClient() {
  FanzaiIPC::munmapBuf(this->shmemBuf, bufferSize);
  FanzaiIPC::unlinkShmem(to_string(this->clientPid).data());
  printf("Client %s has been removed.\n", this->clientName.data());
}
