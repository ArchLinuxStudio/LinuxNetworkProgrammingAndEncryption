#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 100
int main() {
  //创建套接字
  int sock = socket(AF_INET, SOCK_DGRAM, 0);
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

  //接收客户端请求
  struct sockaddr_in clntAddr; //客户端地址信息
  socklen_t nSize = sizeof(clntAddr);
  char buffer[BUF_SIZE]; //缓冲区
  while (1) {
    int strLen = recvfrom(sock, buffer, BUF_SIZE, 0,
                          (struct sockaddr *)&clntAddr, &nSize);
    sendto(sock, buffer, strLen, 0, (struct sockaddr *)&clntAddr, nSize);
  }
  close(sock);
  return 0;
}