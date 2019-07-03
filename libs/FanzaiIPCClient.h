#include "FanzaiIPC.h"

typedef void (*ClientSignalHandler)(char *);

class FanzaiIPCClient {
 private:
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
  FanzaiIPCClient(string serviceName, pid_t clientPid, int bufferLength);
  char *getShmemBuf();
  void updateHandler(ClientSignalHandler newHandler);
  void setRawHandler(RawSigactionHandler handler);
  void wrappedServiceSignalHandler(int signum, siginfo_t *info, void *context);
  void establishConnection();
  void signalService();
  void closeConnection();

  ~FanzaiIPCClient();
};
