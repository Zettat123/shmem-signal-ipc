#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string>

#include "FanzaiIPCService.h"

string SERVICE_NAME = "CHARDEV_SERVICE";

FanzaiIPCService* fis;
int size;

void read_chardev(char* buffer, int size) {
  int i = 0;
  for (i; i < size; i++) buffer[i] = 'a';
  buffer[i] = '\0';
}

int handler(char* rawBuf, pid_t clientPid, FANZAI_SIGNAL_TYPE signalType) {
  char* buf = (char*)rawBuf;
  switch (signalType) {
    case FANZAI_COMMUNICATION:
      printf("Current client pid is %d\n", clientPid);
      printf("Received buffer size is: %d\n", size);
      read_chardev(buf, size);
      printf("Handle signal OK!\n");
      fis->signalClient(clientPid, FANZAI_COMMUNICATION);
      break;

    case FANZAI_ESTABLISH_CONNECTION:
      printf("Received establish params: %d, %d, %d\n", rawBuf[0], rawBuf[1],
             rawBuf[2]);
      size = rawBuf[3];
      break;

    default:
      printf("Wrong signal.\n");
      break;
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
