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
}

int handler(char* buf, int buffer_size) {
  printf("Received buffer size is: %d\n", buffer_size);
  read_chardev(buf, buffer_size);
  printf("Handle signal OK!\n");

  return 0;
}

void RawHandler(int signum, siginfo_t* info, void* context) {
  fis->wrapServiceSignalHandler(signum, info, context);
}

int main() {
  fis = new FanzaiIPCService(SERVICE_NAME, getpid());

  fis->setRawHandler(RawHandler);
  fis->updateHandler(handler);

  for (;;) {
    sleep(10000);
  }

  return 0;
}
