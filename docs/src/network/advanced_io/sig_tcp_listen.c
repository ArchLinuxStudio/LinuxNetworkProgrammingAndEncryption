#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

// 初始化服务端的监听端口。
int initserver(int port);

int listensock;

static void handler(int sig) {
  printf("got signal number:%d\n", sig);
  struct sockaddr_in client;
  socklen_t len = sizeof(client);
  //这里网络不会阻塞了
  int clientsock = accept(listensock, (struct sockaddr *)&client, &len);
  if (clientsock < 0) {
    printf("accept() failed.\n");
  }
  printf("client(socket=%d) connected ok.\n", clientsock);

  //创建子进程 fork返回0则说明位于子进程
  if (!fork()) {
    close(listensock); //子进程中关闭主监听套接字
    char buffer[BUFSIZ] = {0};
    int strLen = read(clientsock, buffer, BUFSIZ); //接收客户端发来的数据
    //向客户端发送数据
    write(clientsock, buffer, strLen);
    close(clientsock); //通信结束，关闭通信套接字
    exit(0); //通信结束 子进程退出，防止进入服务器主循环
  } else {
    close(clientsock);
  }
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("usage: ./tcpselect port\n");
    return -1;
  }

  // 初始化服务端用于监听的socket。
  listensock = initserver(atoi(argv[1]));
  printf("listensock=%d\n", listensock);
  if (listensock < 0) {
    printf("initserver() failed.\n");
    return -1;
  }

  while (1) {
    printf("do something else now\n");
    sleep(1);
  }
  return 0;
}

// 初始化服务端的监听端口。
int initserver(int port) {
  int sock = socket(AF_INET, SOCK_STREAM, 0);

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

  if (sock < 0) {
    printf("socket() failed.\n");
    return -1;
  }

  int opt = 1;
  unsigned int len = sizeof(opt);
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, len);

  struct sockaddr_in servaddr;
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(port);

  if (bind(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
    printf("bind() failed.\n");
    close(sock);
    return -1;
  }

  if (listen(sock, 5) != 0) {
    printf("listen() failed.\n");
    close(sock);
    return -1;
  }

  return sock;
}