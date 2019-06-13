#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "FanzaiIPCClient.h"

#define BUFFER_SIZE 65536 + 10

// #define barrier() __asm__ __volatile__("" ::: "memory")

string CLIENT_NAME = "test_client";
string SERVICE_NAME = "CHARDEV_SERVICE";

FanzaiIPCClient* fic;
int times, size, count;

void handler(char* rawBuf) {
  count++;
  char* buf = (char*)rawBuf;
  printf("%s\n", buf);
  if (count == times) {
    printf("count == times\n");
    fic->closeConnection();
    delete fic;
  }
}

void rawHandler(int signum, siginfo_t* info, void* context) {
  fic->wrapServiceSignalHandler(signum, info, context);
}

int main(int argc, char* argv[]) {
  times = atoi(argv[1]);
  size = atoi(argv[2]);
  count = 0;

  fic = new FanzaiIPCClient(CLIENT_NAME, SERVICE_NAME, getpid(), BUFFER_SIZE);

  fic->setRawHandler(rawHandler);
  fic->updateHandler(handler);

  int i = 0;
  for (i; i < times; i++) {
    int* paramsBuf = (int*)fic->getShmemBuf();
    paramsBuf[0] = size;
    fic->sendMessage();
    sleep(1);
  }

  // fic->closeConnection();

  // delete fic;

  return 0;
}