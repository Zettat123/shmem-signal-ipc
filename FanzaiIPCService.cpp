#include <stdio.h>

#include "FanzaiIPCService.h"

FanzaiIPCService::FanzaiIPCService(string serviceName, pid_t servicePid) {
  this->serviceName = serviceName;
  this->servicePid = servicePid;
  this->serviceSignalHandler = NULL;
}

int FanzaiIPCService::updateHandler(ServiceSignalHandler newHandler) {
  this->serviceSignalHandler = newHandler;
  return FanzaiIPC.insertProcessToMap(serviceName, pid,
                                      SERVICE_MAP_FILE_LOCATION);
}

FanzaiIPCService::~FanzaiIPCService() {
  FanzaiIPC.removeProcessFromMap(this->serviceName);
  printf("Service %s has been removed.\n", this->serviceName);
}