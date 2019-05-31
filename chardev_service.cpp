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

void read_chardev(char* buffer, int size) {
  int i = 0;
  for (i; i < size; i++) buffer[i] = 'a';
  buffer[i] = '\0';
}

int handler(void* rawBuf, int buffer_size) {
  int* paramsBuf = (int*)rawBuf;
  int size = paramsBuf[0];
  char* buf = (char*)rawBuf;
  printf("Received buffer size is: %d\n", size);
  read_chardev(buf, size);
  printf("Handle signal OK!\n");

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
