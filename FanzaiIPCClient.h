#include "FanzaiIPC.h"

typedef void (*pf)() ClientSignalHandler;

class FanzaiIPCClient : public FanzaiIPC {
 private:
  string clientName;
  string serviceName;
  int shmemFd;
  char* shmemBuf;

  int create_shm_fd(char* name, int size);
  char* create_shm_buf(int length, int fd);

 public:
  FanzaiIPCClient(string clientName, string serviceName);
  int sendMessage(pid_t pid, IPCMetadata* metadata,
                  ClientSignalHandler handler);
}