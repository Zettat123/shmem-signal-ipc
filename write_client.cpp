#include <unistd.h>

#include "FanzaiIPCClient.h"

#define BUFFER_SIZE 4096

string CLIENT_NAME = "write_client";
string SERVICE_NAME = "PRINT_SERVICE";

int times, size, count;

FanzaiIPCClient* fic;

void handler(char* rawBuf) {
  printf("Client Handler.\n");
  count++;

  if (count == times) {
    fic->closeConnection();
    delete fic;
  }
}

void fillBuffer(char* buf, int length) {
  for (int i = 0; i < length - 1; i++) {
    buf[i] = (i % 26) + 65;
  }
  buf[length - 1] = '\0';
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

  for (int i = 0; i < times; i++) {
    char* params = (char*)fic->getShmemBuf();
    params[0] = size;
    fillBuffer(params + 1, size);
    fic->sendMessage();
    usleep(1000 * 500);
  }

  while (1) {
    if (count == times) break;
  }

  return 0;
}