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

void FanzaiIPCService::signalClient(pid_t clientPid,
                                    FANZAI_SIGNAL_TYPE signalType) {
  union sigval sv;
  sv.sival_int = signalType;
  sigqueue(clientPid, FANZAI_SIGNAL, sv);
}

void FanzaiIPCService::wrappedServiceSignalHandler(int signum, siginfo_t* info,
                                                   void* context) {
  pid_t clientPid = info->si_pid;
  /**
   * If signalInfo > 0, it is bufferSize.
   * Or it represents other operations.
   */
  int signalType = info->si_value.sival_int;
  string shmemFileName = FANZAI_SHARED_MEMORY_FILE_NAME(clientPid);

  switch (signalType) {
    case FANZAI_COMMUNICATION:
      this->serviceSignalHandler(
          this->ssm[shmemFileName].buf + FANZAI_PARAMS_LENGTH, clientPid,
          FANZAI_COMMUNICATION);

      break;

    case FANZAI_CLOSE_CONNECTION:
      printf("Connection %s will be closed.\n",
             (FANZAI_SHARED_MEMORY_FILE_NAME(clientPid)).data());
      this->closeConnection(clientPid);
      this->signalClient(clientPid, FANZAI_CLOSE_CONNECTION);
      break;

    default:
      if (signalType <= 0) {
        printf("Error signal type.\n");
        return;
      }
      // Establish connection
      int bufferLength = signalType;

      ServiceShmemMap::iterator it = this->ssm.find(shmemFileName);
      if (it == ssm.end()) {
        Shmem sm;
        sm.fd = FanzaiIPC::createShmemFd(shmemFileName, bufferLength);
        sm.buf = FanzaiIPC::createShmemBuf(sm.fd, bufferLength);
        sm.bufferLength = bufferLength;
        close(sm.fd);
        this->ssm[shmemFileName] = sm;
      }

      this->serviceSignalHandler(
          this->ssm[shmemFileName].buf + FANZAI_PARAMS_LENGTH, clientPid,
          FANZAI_ESTABLISH_CONNECTION);

      // Tell client that the connection has been established.
      char* fanzaiParams = this->ssm[shmemFileName].buf;
      fanzaiParams[0] = 1;

      break;
  }
}

void FanzaiIPCService::updateHandler(ServiceSignalHandler newHandler) {
  this->serviceSignalHandler = newHandler;

  struct sigaction sa;
  sa.sa_sigaction = this->rawHandler;
  sa.sa_flags = SA_SIGINFO;
  sigaction(FANZAI_SIGNAL, &sa, NULL);
}

int FanzaiIPCService::closeConnection(pid_t clientPid) {
  string shmemFileName = FANZAI_SHARED_MEMORY_FILE_NAME(clientPid);
  ServiceShmemMap::iterator it = this->ssm.find(shmemFileName);

  FanzaiIPC::munmapBuf((void*)it->second.buf, it->second.bufferLength);
  FanzaiIPC::unlinkShmem(it->first);
  this->ssm.erase(it);

  return 0;
}

FanzaiIPCService::~FanzaiIPCService() {
  FanzaiIPC::removeProcessFromMap(this->serviceName, SERVICE_MAP_FILE_LOCATION);
  ServiceShmemMap::iterator it = this->ssm.begin();
  for (it; it != ssm.end(); it++) {
    FanzaiIPC::munmapBuf(it->second.buf, it->second.bufferLength);
    FanzaiIPC::unlinkShmem(it->first);
  }
  printf("Service %s has been removed.\n", this->serviceName.data());
}
