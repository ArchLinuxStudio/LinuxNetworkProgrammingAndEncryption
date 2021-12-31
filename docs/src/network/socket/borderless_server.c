#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 100

int main() {
  //创建套接字
  int servSock = socket(AF_INET, SOCK_STREAM, 0);

  //绑定套接字
  struct sockaddr_in sockAddr;
  memset(&sockAddr, 0, sizeof(sockAddr));            //每个字节都用0填充
  sockAddr.sin_family = PF_INET;                     //使用IPv4地址
  sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); //具体的IP地址
  sockAddr.sin_port = htons(1234);                   //端口
  if (bind(servSock, (struct sockaddr *)&sockAddr, sizeof(sockAddr)) == -1)
    return -1;

  //进入监听状态
  listen(servSock, 20);

  //接收客户端请求
  struct sockaddr_in clntAddr;
  socklen_t nSize = sizeof(clntAddr);
  char buffer[BUF_SIZE] = {0}; //缓冲区
  int clntSock = accept(servSock, (struct sockaddr *)&clntAddr, &nSize);

  sleep(5); //注意这里，让程序暂停5秒

  //接收客户端发来的数据，并原样返回
  int recvLen = recv(clntSock, buffer, BUF_SIZE, 0);
  send(clntSock, buffer, recvLen, 0);

  //关闭套接字
  close(clntSock);
  close(servSock);
  return 0;
}
