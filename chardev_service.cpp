#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string>

#include "FanzaiIPCService.h"

void read_chardev(char* buffer, int size) {
  int i = 0;
  for (i; i < size; i++) buffer[i] = 'a';
}

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
  update_service_map(SERVICE_NAME, getpid());

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
