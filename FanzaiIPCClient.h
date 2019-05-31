#include "FanzaiIPC.h"

typedef void (*ClientSignalHandler)(void *);
// void (*sa_sigaction)(int, siginfo_t *, void *);

class FanzaiIPCClient {
 private:
  string clientName;
  string serviceName;
  pid_t clientPid;
  pid_t servicePid;
  int bufferSize;
  int shmemFd;
  void *shmemBuf;
  ClientSignalHandler clientSignalHandler;
  RawSigactionHandler rawHandler;

 public:
  FanzaiIPCClient(string clientName, string serviceName, pid_t clientPid,
                  int bufferSize);
  int updateHandler(ClientSignalHandler newHandler);
  void setRawHandler(RawSigactionHandler handler);
  void wrapServiceSignalHandler(int signum, siginfo_t *info, void *context);
  int sendMessage(ClientSignalHandler handler);

  ~FanzaiIPCClient();
};
