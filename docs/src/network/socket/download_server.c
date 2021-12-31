#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 1024
int main() {
  //先检查文件是否存在
  char *filename = "./send.file"; //文件名
  FILE *fp = fopen(filename, "r");
  if (fp == NULL) {
    printf("Cannot open file, press any key to exit!\n");
    return -1;
  }

  int servSock = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in sockAddr;
  memset(&sockAddr, 0, sizeof(sockAddr));
  sockAddr.sin_family = PF_INET;
  sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  sockAddr.sin_port = htons(1234);
  if (bind(servSock, (struct sockaddr *)&sockAddr, sizeof(sockAddr)) == -1)
    return -1;
  listen(servSock, 20);
  struct sockaddr_in clntAddr;
  socklen_t nSize = sizeof(clntAddr);
  int clntSock = accept(servSock, (struct sockaddr *)&clntAddr, &nSize);

  //循环发送数据，直到文件结尾
  char buffer[BUF_SIZE] = {0}; //缓冲区
  int nCount;
  while ((nCount = fread(buffer, 1, BUF_SIZE, fp)) > 0) {
    send(clntSock, buffer, nCount, 0);
  }
  shutdown(clntSock, SHUT_WR); //文件读取完毕，断开输出流，向客户端发送FIN包
  recv(clntSock, buffer, BUF_SIZE, 0); //阻塞，等待客户端接收完毕
  fclose(fp);
  close(clntSock);
  close(servSock);
  return 0;
}