#include "FanzaiIPC.h"

typedef int (*pf)(char*, int) ServiceSignalHandler;

class FanzaiIPCService : public FanzaiIPC {
 private:
  string serviceName;
  pid_t servicePid;
  ServiceSignalHandler serviceSignalHandler;

 public:
  FanzaiIPCService(string serviceName, pid_t servicePid);
  int updateHandler(ServiceSignalHandler newHandler);

  ~FanzaiIPCService();
}