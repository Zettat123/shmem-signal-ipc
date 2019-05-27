#include "FanzaiIPC.h"

typedef int (*pf)(char*, int) handler;

class FanzaiIPCService : public FanzaiIPC {
 private:
  string serviceName;
  pid_t servicePid;
  handler serviceSignalHandler;

 public:
  FanzaiIPCService(string serviceName, pid_t servicePid);
  void updateHandler(handler newHandler);
  int removeServiceFromMap(char* serviceName);
  int updateServiceMap(string serviceName, pid_t pid);
}