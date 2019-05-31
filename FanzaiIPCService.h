#include <signal.h>
#include "FanzaiIPC.h"

typedef int (*ServiceSignalHandler)(void *, int);

typedef struct sm {
  int fd;
  void *buf;
  int length;
} Shmem;

typedef map<pid_t, Shmem> ServiceShmemMap;

class FanzaiIPCService : public FanzaiIPC {
 private:
  string serviceName;
  pid_t servicePid;
  ServiceShmemMap ssm;
  ServiceSignalHandler serviceSignalHandler;
  RawSigactionHandler rawHandler;

 public:
  FanzaiIPCService(string serviceName, pid_t servicePid);
  int updateHandler(ServiceSignalHandler newHandler);
  void wrapServiceSignalHandler(int signum, siginfo_t *info, void *context);
  void setRawHandler(RawSigactionHandler rsh);

  ~FanzaiIPCService();
};
