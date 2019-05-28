#include "FanzaiIPC.h"

typedef void (*ClientSignalHandler)();
// void (*sa_sigaction)(int, siginfo_t *, void *);

class FanzaiIPCClient : public FanzaiIPC {
 private:
  string clientName;
  string serviceName;
  pid_t clientPid;
  pid_t servicePid;
  int bufferSize;
  int shmemFd;
  char* shmemBuf;

 public:
  FanzaiIPCClient(string clientName, string serviceName, pid_t clientPid,
                  int bufferSize);
  int sendMessage(IPCMetadata* metadata, ClientSignalHandler handler);

  ~FanzaiIPCClient();
};
