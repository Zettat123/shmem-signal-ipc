#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void handler(int signum) {
  if (signum == SIGUSR1)
    printf("Signal!\n");
  else
    printf("Error: Wrong Signal. Received signal is: %d.\n", signum);
}

int main(int argc, char* argv[]) {
  int time = atoi(argv[1]);
  printf("Start! Signal is %d\ntime is: %d\n", SIGUSR1, time);
  signal(SIGUSR1, handler);
  sleep(time);
  printf("Finish!\n");
}