#include <arpa/inet.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

// ulimit -n
#define MAXNFDS 1024

// 初始化服务端的监听端口。
int initserver(int port);

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("usage: ./tcppoll port\n");
    return -1;
  }

  // 初始化服务端用于监听的socket。
  int listensock = initserver(atoi(argv[1]));
  printf("listensock=%d\n", listensock);

  if (listensock < 0) {
    printf("initserver() failed.\n");
    return -1;
  }

  int maxfd;                  // fds数组中需要监视的socket的大小。
  struct pollfd fds[MAXNFDS]; // fds存放需要监视的socket。

  // 初始化数组，把全部的fd设置为-1。
  for (int ii = 0; ii < MAXNFDS; ii++)
    fds[ii].fd = -1;
  // 把listensock添加到数组中。
  fds[listensock].fd = listensock;
  // 有数据可读事件，包括新客户端的连接、客户端socket有数据可读和客户端socket断开三种情况。
  fds[listensock].events = POLLIN;
  maxfd = listensock;

  while (1) {

    int infds = poll(fds, maxfd + 1, -1);
    printf("poll infds=%d\n", infds);

    // 返回失败。
    if (infds < 0) {
      printf("poll() failed.\n");
      perror("poll():");
      break;
    }

    // 超时。
    if (infds == 0) {
      printf("poll() timeout.\n");
      continue;
    }

    // 检查有事情发生的socket，包括监听和客户端连接的socket。
    // 这里是客户端的socket事件，每次都要遍历整个集合，因为可能有多个socket有事件。
    for (int eventfd = 0; eventfd <= maxfd; eventfd++) {

      //-1说明并没有置位
      if (fds[eventfd].fd < 0)
        continue;

      //二进制按位与如果为0,说明没有读的事件
      if ((fds[eventfd].revents & POLLIN) == 0)
        continue;

      // 先把revents清空。
      fds[eventfd].revents = 0;

      if (eventfd == listensock) {
        // 如果发生事件的是listensock，表示有新的客户端连上来。
        struct sockaddr_in client;
        socklen_t len = sizeof(client);
        int clientsock = accept(listensock, (struct sockaddr *)&client, &len);

        if (clientsock < 0) {
          printf("accept() failed.\n");
          continue;
        }

        printf("client(socket=%d) connected ok.\n", clientsock);

        if (clientsock > MAXNFDS) {
          printf("clientsock(%d)>MAXNFDS(%d)\n", clientsock, MAXNFDS);
          close(clientsock);
          continue;
        }

        fds[clientsock].fd = clientsock;
        fds[clientsock].events = POLLIN;
        fds[clientsock].revents = 0;

        if (maxfd < clientsock)
          maxfd = clientsock;

        printf("maxfd=%d\n", maxfd);
        continue;
      } else {
        // 客户端有数据过来或客户端的socket连接被断开。
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));

        // 读取客户端的数据。
        ssize_t isize = read(eventfd, buffer, sizeof(buffer));

        // 发生了错误或socket被对方关闭。
        if (isize <= 0) {
          printf("client(eventfd=%d) disconnected.\n", eventfd);
          close(eventfd); // 关闭客户端的socket。
          fds[eventfd].fd = -1;
          // 重新计算maxfd的值，注意，只有当eventfd==maxfd时才需要计算。
          if (eventfd == maxfd) {
            for (int ii = maxfd; ii > 0; ii--) {
              if (fds[ii].fd != -1) {
                maxfd = ii;
                break;
              }
            }
            printf("maxfd=%d\n", maxfd);
          }
          continue;
        }

        printf("recv(eventfd=%d,size=%zd):%s\n", eventfd, isize, buffer);
        // 把收到的报文发回给客户端。
        write(eventfd, buffer, strlen(buffer));
      }
    }
  }

  return 0;
}

// 初始化服务端的监听端口。
int initserver(int port) {
  int sock = socket(AF_INET, SOCK_STREAM, 0);
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