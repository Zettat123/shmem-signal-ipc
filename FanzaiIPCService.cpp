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

  this->serviceSignalHandler(this->ssm[clientPid].buf, bufferSize);
  union sigval sv;
  sv.sival_int = 306;
  sigqueue(clientPid, FANZAI_SIGNAL, sv);
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
  ServiceShmemMap::iterator it = this->ssm.begin();
  for (it; it != ssm.end(); it++) {
    FanzaiIPC::munmapBuf(it->second.buf, it->second.length);
    FanzaiIPC::unlinkShmem(to_string(it->first));
  }
  printf("Service %s has been removed.\n", this->serviceName.data());
}
