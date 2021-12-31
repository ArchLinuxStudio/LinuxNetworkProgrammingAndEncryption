#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 100
char buffer[BUF_SIZE]; //缓冲区
int sock;

//接收客户端请求
struct sockaddr_in clntAddr; //客户端地址信息
socklen_t nSize = sizeof(clntAddr);

static void handler(int sig) {
  printf("got signal number:%d\n", sig);

  int strLen =
      recvfrom(sock, buffer, BUF_SIZE, 0, (struct sockaddr *)&clntAddr, &nSize);
  sendto(sock, buffer, strLen, 0, (struct sockaddr *)&clntAddr, nSize);
}

int main() {
  //创建套接字
  sock = socket(AF_INET, SOCK_DGRAM, 0);
  struct sigaction sa;
  /* Establish handler */
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  sa.sa_handler = handler;
  if (sigaction(SIGIO, &sa, NULL) == -1) {
    perror("sigaction()\n");
    exit(1);
  }

  /* Set owner process */
  if (fcntl(sock, F_SETOWN, getpid()) == -1) {
    perror("fcntl() / F_SETOWN\n");
    exit(1);
  }

  /* Enable "I/O possible" signaling and make I/O nonblocking */
  int flags = fcntl(sock, F_GETFL);
  if (fcntl(sock, F_SETFL, flags | O_ASYNC | O_NONBLOCK) == -1) {
    perror("fcntl() / F_SETFL\n");
    exit(1);
  }

  //绑定套接字
  struct sockaddr_in servAddr;
  memset(&servAddr, 0, sizeof(servAddr));       //每个字节都用0填充
  servAddr.sin_family = AF_INET;                //使用IPv4地址
  servAddr.sin_addr.s_addr = htonl(INADDR_ANY); //自动获取IP地址
  servAddr.sin_port = htons(1234);              //端口
  if (bind(sock, (struct sockaddr *)&servAddr, sizeof(servAddr)) == -1) {
    printf("error!%s", strerror(errno));
    return -1;
  }

  while (1) {
    sleep(50);
  }
  close(sock);
  return 0;
}