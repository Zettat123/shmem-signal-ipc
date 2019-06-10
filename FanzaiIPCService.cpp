#include <signal.h>
#include <stdio.h>
#include <unistd.h>

#include "FanzaiIPCService.h"

FanzaiIPCService::FanzaiIPCService(string serviceName, pid_t servicePid) {
  this->serviceName = serviceName;
  this->servicePid = servicePid;
  this->serviceSignalHandler = NULL;

  if (FanzaiIPC::insertProcessToMap(serviceName, servicePid,
                                    SERVICE_MAP_FILE_LOCATION) == -1) {
    throw "Same service name error\n";
  }

  ServiceShmemMap ssm;
  this->ssm = ssm;
}

void FanzaiIPCService::setRawHandler(RawSigactionHandler rsh) {
  this->rawHandler = rsh;
}

void FanzaiIPCService::wrapServiceSignalHandler(int signum, siginfo_t* info,
                                                void* context) {
  pid_t clientPid = info->si_pid;
  int bufferSize = info->si_value.sival_int;

  ServiceShmemMap::iterator it = this->ssm.find(clientPid);
  if (it == ssm.end()) {
    Shmem sm;
    sm.fd = FanzaiIPC::createShmemFd(to_string(clientPid), bufferSize);
    sm.buf = FanzaiIPC::createShmemBuf(sm.fd, bufferSize);
    sm.length = bufferSize;
    close(sm.fd);
    this->ssm[clientPid] = sm;
  }

  int* fanzaiParams = (int*)this->ssm[clientPid].buf;

  if (fanzaiParams[0] == 0) {
    this->serviceSignalHandler(this->ssm[clientPid].buf + FANZAI_PARAMS_LENGTH,
                               bufferSize);
    union sigval sv;
    sv.sival_int = 0;
    sigqueue(clientPid, FANZAI_SIGNAL, sv);
  } else if (fanzaiParams[0] == 1) {
    printf("Connection will be closed.\n");
    this->closeConnection(clientPid);
    union sigval sv;
    sv.sival_int = 1;
    sigqueue(clientPid, FANZAI_SIGNAL, sv);
    printf("Connection close completed.\n");
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

int FanzaiIPCService::closeConnection(pid_t clientPid) {
  ServiceShmemMap::iterator it = this->ssm.find(clientPid);
  FanzaiIPC::munmapBuf(it->second.buf, it->second.length);
  FanzaiIPC::unlinkShmem(to_string(it->first));

  return 0;
}

FanzaiIPCService::~FanzaiIPCService() {
  FanzaiIPC::removeProcessFromMap(this->serviceName, SERVICE_MAP_FILE_LOCATION);
  ServiceShmemMap::iterator it = this->ssm.begin();
  for (it; it != ssm.end(); it++) {
    FanzaiIPC::munmapBuf(it->second.buf, it->second.length);
    FanzaiIPC::unlinkShmem(to_string(it->first));
  }
  printf("Service %s has been removed.\n", this->serviceName.data());
}
