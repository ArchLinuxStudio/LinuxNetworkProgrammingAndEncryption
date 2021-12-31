#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define BUF_SIZE 1024
int main() {
  //先输入文件名，看文件是否能创建成功
  char filename[100] = {0}; //文件名
  printf("Input filename to save: ");
  fgets(filename, 100, stdin);
  filename[strlen(filename) - 1] = '\0'; //去除fgets后缀\n
  FILE *fp = fopen(filename, "w");
  if (fp == NULL) {
    printf("Cannot open file, press any key to exit!\n");
    return -1;
  }
  int sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  struct sockaddr_in sockAddr;
  memset(&sockAddr, 0, sizeof(sockAddr));
  sockAddr.sin_family = PF_INET;
  sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  sockAddr.sin_port = htons(1234);
  if (connect(sock, (struct sockaddr *)&sockAddr, sizeof(sockAddr)) == -1)
    return -1;
  //循环接收数据，直到文件传输完毕
  char buffer[BUF_SIZE] = {0}; //文件缓冲区
  int nCount;
  while ((nCount = recv(sock, buffer, BUF_SIZE, 0)) > 0) {
    fwrite(buffer, 1, nCount, fp);
  }
  puts("File transfer success!");
  //文件接收完毕后直接关闭套接字，无需调用shutdown()
  fclose(fp);
  close(sock);
  return 0;
}