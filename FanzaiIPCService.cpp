#include "FanzaiIPCService.h"

FanzaiIPCService::FanzaiIPCService(string serviceName, pid_t servicePid) {
  this.serviceName = serviceName;
  this.servicePid = servicePid;
  this.serviceSignalHandler = NULL;
}

void FanzaiIPCService::updateHandler(handler newHandler) {
  this.serviceSignalHandler = newHandler;
}

int FanzaiIPCService::updateServiceMap(string serviceName, pid_t pid) {
  FanzaiServiceMap fsm = read_map_from_file();
  fsm[serviceName] = pid;
  write_map_to_file(fsm);

  return 0;
}

int FanzaiIPCService::removeServiceFromMap(char* serviceName) {
  FanzaiServiceMap fsm = read_map_from_file();
  fsm.erase(serviceName);
  write_map_to_file(fsm);

  return 0;
}
