#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static volatile sig_atomic_t gotSigio = 0;

static void handler(int sig) { gotSigio = 1; }

int main(int argc, char *argv[]) {
  int flags, cnt;
  char ch;
  struct sigaction sa;
  int done;

  /* Establish handler */
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  sa.sa_handler = handler;
  if (sigaction(SIGIO, &sa, NULL) == -1) {
    perror("sigaction()\n");
    exit(1);
  }

  /* Set owner process */
  if (fcntl(STDIN_FILENO, F_SETOWN, getpid()) == -1) {
    perror("fcntl() / F_SETOWN\n");
    exit(1);
  }

  /* Enable "I/O possible" signaling and make I/O nonblocking */
  flags = fcntl(STDIN_FILENO, F_GETFL);
  if (fcntl(STDIN_FILENO, F_SETFL, flags | O_ASYNC | O_NONBLOCK) == -1) {
    perror("fcntl() / F_SETFL\n");
    exit(1);
  }

  for (done = 0, cnt = 0; !done; cnt++) {
    sleep(1);
    printf("Round: %d\n", cnt);
    if (gotSigio) {
      gotSigio = 0;
      /* Read all available input until error (probably EAGAIN)
         or EOF */
      while (read(STDIN_FILENO, &ch, 1) > 0 && !done) {
        printf("read %c\n", ch);
        done = ch == '#';
      }
    }
  }
  exit(0);
}