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

void* FanzaiIPCClient::getShmemBuf() {
  return this->shmemBuf + FANZAI_PARAMS_LENGTH;
}

void FanzaiIPCClient::wrapServiceSignalHandler(int signum, siginfo_t* info,
                                               void* context) {
  int type = info->si_value.sival_int;

  printf("type = %d\n", type);

  switch (type) {
    case 0:
      this->clientSignalHandler(this->shmemBuf + FANZAI_PARAMS_LENGTH);
      break;

    case 1:
      this->removeShmem();
      break;
  }
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
  int* fanzaiParams = (int*)this->shmemBuf;
  fanzaiParams[0] = 0;

  union sigval sv;
  sv.sival_int = this->bufferSize;
  sigqueue(this->servicePid, FANZAI_SIGNAL, sv);
}

int FanzaiIPCClient::closeConnection() {
  int* fanzaiParams = (int*)this->shmemBuf;
  fanzaiParams[0] = 1;

  union sigval sv;
  sv.sival_int = this->bufferSize;
  sigqueue(this->servicePid, FANZAI_SIGNAL, sv);
}

int FanzaiIPCClient::removeShmem() {
  FanzaiIPC::munmapBuf(this->shmemBuf, bufferSize);
  FanzaiIPC::unlinkShmem(to_string(this->clientPid).data());
  printf("Connection closed.\n");

  return 0;
}

FanzaiIPCClient::~FanzaiIPCClient() {
  printf("Client %s has been removed.\n", this->clientName.data());
}
