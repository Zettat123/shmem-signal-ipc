#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "fanzai_ipc.h"

#define C2S_MAX_SIZE 128
#define S2C_MAX_SIZE 65536 + 10
#define C2SSHM "/c2sshm"
#define S2CSHM "/s2cshm"

void read_chardev(char* buffer, int size) { syscall(392, buffer, size); }

void handler(int signum, siginfo_t* info, void* context) {
  if (signum == SIGUSR1)
    printf("signal: %d\n", signum);
  else
    printf("error\n");

  if (context) {
    printf("content: %d\n", info->si_int);
    printf("content: %d\n", info->si_value.sival_int);
  }
}

int main() {
  struct sigaction act;
  act.sa_sigaction = handler;
  act.sa_flags = SA_SIGINFO;

  sigaction(SIGUSR1, &act, NULL);

  for (;;) {
    sleep(10000);
  }

  int c2sfd = create_shm_fd(C2SSHM, C2S_MAX_SIZE);

  //   if (c2sfd < 0) {
  //     c2sfd = shm_open(C2SSHM, O_RDWR, 0777);
  //     if (c2sfd < 0) {
  //       printf("error open %s object\n", C2SSHM);
  //       return 0;
  //     }
  //     ftruncate(c2sfd, C2S_MAX_SIZE);
  //   }
  int* c2sbuf = (int*)create_shm_buf(C2S_MAX_SIZE, c2sfd);
  close(c2sfd);

  int s2cfd = create_shm_fd(S2CSHM, S2C_MAX_SIZE);
  //   if (s2cfd < 0) {
  //     s2cfd = shm_open(S2CSHM, O_RDWR, 0777);
  //     if (s2cfd < 0) {
  //       printf("error open %s object\n", S2CSHM);
  //       return 0;
  //     }
  //     ftruncate(s2cfd, S2C_MAX_SIZE);
  //   }
  char* s2cbuf = (char*)create_shm_buf(S2C_MAX_SIZE, s2cfd);
  close(s2cfd);

  int lastc2scounter = c2sbuf[0];
  s2cbuf[0] = 0;
  // while (1) {
  //   if (c2sbuf[0] == lastc2scounter) {
  //     // printf("No request.\n");
  //   } else {
  //     printf("Request detected, counter is %d\n", c2sbuf[0]);
  //     int buffer_size = c2sbuf[1];
  //     read_chardev(s2cbuf + sizeof(char), buffer_size);
  //     s2cbuf[0] = 1 - s2cbuf[0];
  //     lastc2scounter = c2sbuf[0];
  //   }

  //   // sleep(1);
  // }

  shm_unlink(C2SSHM);
  shm_unlink(S2CSHM);
  return 0;
}
