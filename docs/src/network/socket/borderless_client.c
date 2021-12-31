#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
int main() {
  //向服务器发起请求
  struct sockaddr_in sockAddr;
  memset(&sockAddr, 0, sizeof(sockAddr)); //每个字节都用0填充
  sockAddr.sin_family = PF_INET;
  sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  sockAddr.sin_port = htons(1234);
  //创建套接字
  int sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (connect(sock, (struct sockaddr *)&sockAddr, sizeof(sockAddr)) == -1)
    return -1;
  //获取用户输入的字符串并发送给服务器
  char bufSend[BUFSIZ] = {0};
  printf("Input a string: ");
  fgets(bufSend, BUFSIZ, stdin);

  for (int i = 0; i < 3; i++) {
    send(sock, bufSend, strlen(bufSend), 0);
  }
  //接收服务器传回的数据
  char bufRecv[BUFSIZ] = {0};
  recv(sock, bufRecv, BUFSIZ, 0);
  //输出接收到的数据
  printf("Message form server: %s\n", bufRecv);
  close(sock); //关闭套接字
  return 0;
}