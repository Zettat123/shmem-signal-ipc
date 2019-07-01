#include <signal.h>
#include "FanzaiIPC.h"

typedef int (*ServiceSignalHandler)(char *, int);

typedef struct sm {
  int fd;
  char *buf;
  int bufferLength;
} Shmem;

typedef map<string, Shmem> ServiceShmemMap;

class FanzaiIPCService : public FanzaiIPC {
 private:
  string serviceName;
  pid_t servicePid;
  ServiceShmemMap ssm;
  ServiceSignalHandler serviceSignalHandler;
  RawSigactionHandler rawHandler;
  int closeConnection(pid_t clientPid);

 public:
  FanzaiIPCService(string serviceName, pid_t servicePid);
  int updateHandler(ServiceSignalHandler newHandler);
  void wrapServiceSignalHandler(int signum, siginfo_t *info, void *context);
  void setRawHandler(RawSigactionHandler rsh);

  ~FanzaiIPCService();
};
