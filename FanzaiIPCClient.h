#include "FanzaiIPC.h"

typedef void (*pf)() ClientSignalHandler;
// void (*sa_sigaction)(int, siginfo_t *, void *);

class FanzaiIPCClient : public FanzaiIPC {
 private:
  string clientName;
  string serviceName;
  pid_t clientPid;
  pid_t servicePid;
  int shmemFd;
  char* shmemBuf;

 public:
  FanzaiIPCClient(string clientName, string serviceName, pid_t clientPid);
  int sendMessage(IPCMetadata* metadata, ClientSignalHandler handler);
}