#include <stdio.h>
#include <sys/socket.h>
int main() {
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  int optVal;
  socklen_t optLen = sizeof(int);
  getsockopt(sock, SOL_SOCKET, SO_SNDBUF, &optVal, &optLen);
  printf("Buffer length: %d\n", optVal);
  return 0;
}