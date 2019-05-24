#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "fanzai_ipc.h"

#define C2S_MAX_SIZE 128
#define S2C_MAX_SIZE 65536 + 10
#define C2SSHM "/c2sshm"
#define S2CSHM "/s2cshm"

int main(int argc, char* argv[]) {
  int server_pid = atoi(argv[1]);
  int times = atoi(argv[2]);
  int size = atoi(argv[3]);

  int c2sfd = create_shm_fd(C2SSHM, C2S_MAX_SIZE);
  int* c2sbuf = (int*)create_shm_buf(C2S_MAX_SIZE, c2sfd);
  close(c2sfd);

  int s2cfd = create_shm_fd(S2CSHM, S2C_MAX_SIZE);
  char* s2cbuf = (char*)create_shm_buf(S2C_MAX_SIZE, s2cfd);
  close(s2cfd);

  int i = 0;

  struct timeval tv0, tv1;
  gettimeofday(&tv0, NULL);

  for (i; i < times; i++) {
    c2sbuf[0] = size;
    char lasts2ccounter = s2cbuf[0];
    kill(server_pid, FANZAI_SIGNAL);
    while (s2cbuf[0] == lasts2ccounter)
      ;
    s2cbuf[sizeof(char) + size] = '\0';
    printf("%s\n", s2cbuf + sizeof(char));

    // sleep(1);
  }

  gettimeofday(&tv1, NULL);

  // printf("time0 = %ld.%ld\ntime1 = %ld.%ld\nInterval = %ld us\n", tv0.tv_sec,
  //        tv0.tv_usec, tv1.tv_sec, tv1.tv_usec,
  //        (tv1.tv_sec - tv0.tv_sec) * 1000000 + (tv1.tv_usec - tv0.tv_usec));

  return 0;
}