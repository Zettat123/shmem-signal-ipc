#include <unistd.h>

#include "FanzaiIPCService.h"

string SERVICE_NAME = "PRINT_SERVICE";
FanzaiIPCService* fis;

int handler(char* rawBuf, int bufferLength, pid_t clientPid) {
  printf("%s\n", rawBuf + 1);

  fis->signalClient(clientPid, FANZAI_COMMUNICATION);

  return 0;
}

void rawHandler(int signum, siginfo_t* info, void* context) {
  fis->wrapServiceSignalHandler(signum, info, context);
}

int main() {
  fis = new FanzaiIPCService(SERVICE_NAME, getpid());

  fis->setRawHandler(rawHandler);
  fis->updateHandler(handler);

  for (;;) {
    sleep(10000);
  }

  return 0;
}
