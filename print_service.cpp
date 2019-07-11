#include <unistd.h>

#include "FanzaiIPCService.h"

string SERVICE_NAME = "PRINT_SERVICE";
FanzaiIPCService* fis;

int handler(char* rawBuf, pid_t clientPid, FANZAI_SIGNAL_TYPE signalType) {
  if (signalType == FANZAI_COMMUNICATION) {
    printf("%s\n", rawBuf);

    fis->signalClient(clientPid, FANZAI_COMMUNICATION);
  }

  return 0;
}

void rawHandler(int signum, siginfo_t* info, void* context) {
  fis->wrappedServiceSignalHandler(signum, info, context);
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
