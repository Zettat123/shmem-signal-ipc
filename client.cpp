#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "FanzaiIPCClient.h"

#define BUFFER_SIZE 65536 + 10

string CLIENT_NAME = "/test_client";

void handler() {}

int main(int argc, char* argv[]) {
  int times = atoi(argv[1]);
  int size = atoi(argv[2]);

  FanzaiIPCClient fic(CLIENT_NAME, "CHARDEV_SERVICE", getpid(), BUFFER_SIZE);

  IPCMetadata* metadataPtr = (IPCMetadata*)malloc(sizeof(IPCMetadata));
  metadataPtr->clientName = CLIENT_NAME;
  metadataPtr->bufferSize = BUFFER_SIZE;
  metadataPtr->type = 0;

  fic.sendMessage(metadataPtr, handler);

  // int i = 0;

  // struct timeval tv0, tv1;
  // gettimeofday(&tv0, NULL);

  // for (i; i < times; i++) {
  //   c2sbuf[0] = size;
  //   char lasts2ccounter = s2cbuf[0];
  //   kill(server_pid, FANZAI_SIGNAL);
  //   while (s2cbuf[0] == lasts2ccounter)
  //     ;
  //   s2cbuf[sizeof(char) + size] = '\0';
  //   printf("%s\n", s2cbuf + sizeof(char));

  //   // sleep(1);
  // }

  // gettimeofday(&tv1, NULL);

  // printf("time0 = %ld.%ld\ntime1 = %ld.%ld\nInterval = %ld us\n", tv0.tv_sec,
  //        tv0.tv_usec, tv1.tv_sec, tv1.tv_usec,
  //        (tv1.tv_sec - tv0.tv_sec) * 1000000 + (tv1.tv_usec - tv0.tv_usec));

  return 0;
}