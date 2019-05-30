#include <signal.h>
#include "FanzaiIPC.h"

typedef int (*ServiceSignalHandler)(char *, int);
typedef void (*RawSigactionHandler)(int, siginfo_t *, void *);

class FanzaiIPCService : public FanzaiIPC {
 private:
  string clientName;
  string serviceName;
  pid_t clientPid;
  pid_t servicePid;
  int shmemFd;
  char *shmemBuf;
  ServiceSignalHandler serviceSignalHandler;
  RawSigactionHandler rawHandler;

 public:
  FanzaiIPCService(string serviceName, pid_t servicePid);
  int updateHandler(ServiceSignalHandler newHandler);
  void wrapServiceSignalHandler(int signum, siginfo_t *info, void *context);
  void setRawHandler(RawSigactionHandler rsh);

  ~FanzaiIPCService();
};
