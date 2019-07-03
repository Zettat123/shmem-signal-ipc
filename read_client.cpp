#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "FanzaiIPCClient.h"

#define BUFFER_SIZE 65536 + 10

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
  fic->wrappedServiceSignalHandler(signum, info, context);
}

int main(int argc, char* argv[]) {
  times = atoi(argv[1]);
  size = atoi(argv[2]);
  count = 0;

  fic = new FanzaiIPCClient(SERVICE_NAME, getpid(), BUFFER_SIZE);

  fic->setRawHandler(rawHandler);
  fic->updateHandler(handler);

  char* params = fic->getShmemBuf();
  params[0] = 74;
  params[1] = 75;
  params[2] = 78;
  params[3] = size;
  fic->establishConnection();

  int i = 0;
  for (i; i < times; i++) {
    fic->signalService();
    sleep(1);
  }

  return 0;
}