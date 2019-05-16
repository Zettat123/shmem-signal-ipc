#include "fanzai_ipc.h"

#define C2S_MAX_SIZE 128
#define S2C_MAX_SIZE 65536 + 10
#define C2SSHM "/c2sshm"
#define S2CSHM "/s2cshm"

int main(int argc, char* argv[]) {
  int times = atoi(argv[1]);
  int size = atoi(argv[2]);

  int c2sfd = create_shm_fd(C2SSHM, C2S_MAX_SIZE);
  int* c2sbuf = (int*)create_shm_buf(C2S_MAX_SIZE, c2sfd);
  close(c2sfd);

  int s2cfd = create_shm_fd(S2CSHM, S2C_MAX_SIZE);
  char* s2cbuf = (char*)create_shm_buf(S2C_MAX_SIZE, s2cfd);
  close(s2cfd);

  for (i; i < times; i++) {
    c2sbuf[0] = 1 - c2sbuf[0];
    c2sbuf[1] = size;
    char lasts2ccounter = s2cbuf[0];
    while (s2cbuf[0] == lasts2ccounter)
      ;
    printf("%s\n", s2cbuf + sizeof(char));

    // sleep(1);
  }

  return 0;
}