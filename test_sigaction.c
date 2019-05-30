#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

char str[6] = "Hello\0";

void handler(int signum, siginfo_t* info, void* context) {
  if (signum == SIGUSR1) {
    char* data = (char*)(info->si_value.sival_ptr);
    printf("%s\n", data);
  }
}

int main() {
  int input;

  struct sigaction sa;
  sa.sa_sigaction = handler;
  sa.sa_flags = SA_SIGINFO;
  sigaction(SIGUSR1, &sa, NULL);

  while (scanf("%d", &input) != EOF) {
    if (input == 5) {
      union sigval sv;
      sv.sival_ptr = (void*)str;
      sigqueue(getpid(), SIGUSR1, sv);
    }
  }
}
