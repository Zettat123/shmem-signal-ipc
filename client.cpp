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

void handler(char* rawBuf) {
  char* buf = (char*)rawBuf;
  printf("%s\n", buf);
}

void rawHandler(int signum, siginfo_t* info, void* context) {
  fic->wrapServiceSignalHandler(signum, info, context);
}

void fuckHandler(int signum, siginfo_t* info, void* context) {
  printf("FUCK\n");
}

int main(int argc, char* argv[]) {
  int times = atoi(argv[1]);
  int size = atoi(argv[2]);

  fic = new FanzaiIPCClient(CLIENT_NAME, SERVICE_NAME, getpid(), BUFFER_SIZE);

  fic->setRawHandler(rawHandler);
  fic->updateHandler(handler);

  //*********************************
  // struct sigaction sa;
  // sa.sa_sigaction = fuckHandler;
  // sa.sa_flags = SA_SIGINFO;
  // sigaction(FANZAI_SIGNAL, &sa, NULL);
  //*********************************

  int i = 0;
  for (i; i < times; i++) {
    int* paramsBuf = (int*)fic->getShmemBuf();
    paramsBuf[0] = size;
    // barrier();
    fic->sendMessage();
    sleep(1);
  }

  fic->closeConnection();

  // delete fic;

  return 0;
}