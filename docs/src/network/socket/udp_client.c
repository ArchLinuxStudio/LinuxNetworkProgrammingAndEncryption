
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 100
int main() {
  //创建套接字
  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  //服务器地址信息
  struct sockaddr_in servAddr;
  memset(&servAddr, 0, sizeof(servAddr)); //每个字节都用0填充
  servAddr.sin_family = PF_INET;
  servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  servAddr.sin_port = htons(1234);
  //不断获取用户输入并发送给服务器，然后接受服务器数据
  struct sockaddr fromAddr;
  socklen_t addrLen = sizeof(fromAddr);
  while (1) {
    char buffer[BUF_SIZE] = {0};
    printf("Input a string: ");
    fgets(buffer, BUF_SIZE, stdin);
    sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)&servAddr,
           sizeof(servAddr));
    int strLen = recvfrom(sock, buffer, BUF_SIZE, 0, &fromAddr, &addrLen);

    struct sockaddr_in client;
    socklen_t clientAddrLen = sizeof(client);

    getsockname(sock, (struct sockaddr *)&client, &clientAddrLen);
    printf("client:client address = %s:%d , length: %u \n",
           inet_ntoa(client.sin_addr), client.sin_port, clientAddrLen);

    buffer[strLen] = 0;
    printf("Message form server: %s\n", buffer);
  }
  close(sock);
  return 0;
}