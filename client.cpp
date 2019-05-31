#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "FanzaiIPCClient.h"

#define BUFFER_SIZE 4 + 2

string CLIENT_NAME = "test_client";
string SERVICE_NAME = "CHARDEV_SERVICE";

FanzaiIPCClient* fic;

void handler(void* rawBuf) {
  char* buf = (char*)rawBuf;
  printf("%s\n", buf);

  delete fic;
}

void rawHandler(int signum, siginfo_t* info, void* context) {
  fic->wrapServiceSignalHandler(signum, info, context);
}

int main(int argc, char* argv[]) {
  int times = atoi(argv[1]);
  int size = atoi(argv[2]);

  fic = new FanzaiIPCClient(CLIENT_NAME, SERVICE_NAME, getpid(), BUFFER_SIZE);
  fic->setRawHandler(rawHandler);
  fic->updateHandler(handler);

  fic->sendMessage(handler);

  return 0;
}