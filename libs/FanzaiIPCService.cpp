#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <string>

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
  string shmemPid = to_string(clientPid);

  switch (signalType) {
    case FANZAI_COMMUNICATION:
      this->serviceSignalHandler(
          this->ssm[shmemPid].buf + FANZAI_PARAMS_LENGTH, clientPid,
          FANZAI_COMMUNICATION);

      break;

    case FANZAI_CLOSE_CONNECTION:
      printf("Connection %s will be closed.\n",
             shmemPid.data());
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

      ServiceShmemMap::iterator it = this->ssm.find(shmemPid);
      if (it == ssm.end()) {
        Shmem sm;
        sm.id = FanzaiIPC::createShmemID(clientPid, bufferLength);
        sm.buf = FanzaiIPC::createShmemBuf(sm.id);
        sm.bufferLength = bufferLength;
        this->ssm[shmemPid] = sm;
      }

      this->serviceSignalHandler(
          this->ssm[shmemPid].buf + FANZAI_PARAMS_LENGTH, clientPid,
          FANZAI_ESTABLISH_CONNECTION);

      // Tell client that the connection has been established.
      char* fanzaiParams = this->ssm[shmemPid].buf;
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
  string shmemPid = to_string(clientPid);
  ServiceShmemMap::iterator it = this->ssm.find(shmemPid);

  FanzaiIPC::removeShmemBuf((void*)it->second.buf);
  //FanzaiIPC::removeShmem(it->second.id);
  this->ssm.erase(it);

  return 0;
}

FanzaiIPCService::~FanzaiIPCService() {
  FanzaiIPC::removeProcessFromMap(this->serviceName, SERVICE_MAP_FILE_LOCATION);
  ServiceShmemMap::iterator it = this->ssm.begin();
  for (it; it != ssm.end(); it++) {
    FanzaiIPC::removeShmemBuf((void*)it->second.buf);
    //FanzaiIPC::removeShmem(it->second.id);
  }
  printf("Service %s has been removed.\n", this->serviceName.data());
}
