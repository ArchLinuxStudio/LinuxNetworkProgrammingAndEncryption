#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

char buf[200000];

int main() {
  int ntowrite, nwrite;
  char *ptr;
  ntowrite = read(STDIN_FILENO, buf, sizeof(buf));
  fprintf(stderr, "read %d bytes\n", ntowrite);

  fcntl(STDOUT_FILENO, F_SETFL, fcntl(STDOUT_FILENO, F_GETFL, 0) | O_NONBLOCK);

  ptr = buf;

  while (ntowrite > 0) {
    //每次轮询需重置errno
    errno = 0;
    nwrite = write(STDOUT_FILENO, ptr, ntowrite);
    fprintf(stderr, "nwirte = %d, errno = %d, err: %s\n", nwrite, errno,
            strerror(errno));

    if (nwrite > 0) {
      ptr += nwrite;
      ntowrite -= nwrite;
    }
  }

  //重置标志位
  int oldfl;
  oldfl = fcntl(STDOUT_FILENO, F_GETFL);
  if (oldfl == -1) {
    /* 错误处理 省略 上同 */
  }

  // unset the O_NONBLOCK flag.
  fcntl(STDOUT_FILENO, F_SETFL, oldfl & ~O_NONBLOCK);
}