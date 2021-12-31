#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
void int_handler(int signum) { printf("int handler %d\n", signum); }

int main(int argc, char **argv) {
  char buf[100];
  ssize_t ret;
  struct sigaction oldact;
  struct sigaction act;
  act.sa_handler = int_handler;
  act.sa_flags = 0;
  // comment it or not
  // 低速系统调用可使用SA_RESTART标志位，在被中断后重启
  // act.sa_flags |= SA_RESTART;
  sigemptyset(&act.sa_mask);
  if (-1 == sigaction(SIGINT, &act, &oldact)) {
    printf("sigaction failed!\n");
    return -1;
  }
  bzero(buf, 100);
  ret = read(STDIN_FILENO, buf, 10);
  if (ret == -1) {
    printf("read error %s\n", strerror(errno));
  }
  printf("read %zd bytes, content is %s\n", ret, buf);
  sleep(3);
  return 0;
}