#include "FanzaiIPC.h"

typedef void (*ClientSignalHandler)(char *);
// void (*sa_sigaction)(int, siginfo_t *, void *);

class FanzaiIPCClient {
 private:
  string clientName;
  string serviceName;
  pid_t clientPid;
  pid_t servicePid;
  int bufferLength;
  int shmemFd;
  char *shmemBuf;
  string shmemFileName;
  ClientSignalHandler clientSignalHandler;
  RawSigactionHandler rawHandler;
  int removeShmem();

 public:
  FanzaiIPCClient(string clientName, string serviceName, pid_t clientPid,
                  int bufferLength);
  void *getShmemBuf();
  int updateHandler(ClientSignalHandler newHandler);
  void setRawHandler(RawSigactionHandler handler);
  void wrapServiceSignalHandler(int signum, siginfo_t *info, void *context);
  int sendMessage();
  int closeConnection();

  ~FanzaiIPCClient();
};
