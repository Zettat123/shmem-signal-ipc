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

int* c2sbuf;
char* s2cbuf;

void read_chardev(char* buffer, int size) { syscall(392, buffer, size); }

void handler(int signum) {
  if (signum == FANZAI_SIGNAL) {
    printf("Server pid is: %d\n", getpid());
    printf("Signal: %d\n", signum);
    int buffer_size = c2sbuf[0];
    printf("Received buffer size is: %d\n", buffer_size);
    read_chardev(s2cbuf + sizeof(char), buffer_size);
    s2cbuf[0] = 1 - s2cbuf[0];
    printf("Handle signal OK!\n");
  }

  else
    printf("Error: Wrong Signal. Received signal is: %d.\n", signum);
}

int main() {
  int c2sfd = create_shm_fd(C2SSHM, C2S_MAX_SIZE);
  c2sbuf = (int*)create_shm_buf(C2S_MAX_SIZE, c2sfd);
  close(c2sfd);

  int s2cfd = create_shm_fd(S2CSHM, S2C_MAX_SIZE);
  s2cbuf = (char*)create_shm_buf(S2C_MAX_SIZE, s2cfd);
  close(s2cfd);

  signal(SIGUSR1, handler);

  for (;;) {
    sleep(10000);
  }

  shm_unlink(C2SSHM);
  shm_unlink(S2CSHM);
  return 0;
}
