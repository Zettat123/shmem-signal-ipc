#include <signal.h>
#include "FanzaiIPC.h"

// raw buffer; client pid; signal type
typedef int (*ServiceSignalHandler)(char *, pid_t, FANZAI_SIGNAL_TYPE);

typedef struct sm {
  int fd;
  char *buf;
  int bufferLength;
} Shmem;

typedef map<string, Shmem> ServiceShmemMap;

class FanzaiIPCService {
 private:
  string serviceName;
  pid_t servicePid;
  ServiceShmemMap ssm;
  ServiceSignalHandler serviceSignalHandler;
  RawSigactionHandler rawHandler;
  int closeConnection(pid_t clientPid);

 public:
  FanzaiIPCService(string serviceName, pid_t servicePid);
  void updateHandler(ServiceSignalHandler newHandler);
  void wrappedServiceSignalHandler(int signum, siginfo_t *info, void *context);
  void setRawHandler(RawSigactionHandler rsh);
  void signalClient(pid_t clientPid, FANZAI_SIGNAL_TYPE signalType);

  ~FanzaiIPCService();
};
