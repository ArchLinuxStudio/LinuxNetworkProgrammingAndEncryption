#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
int main() {

  //向服务器（特定的IP和端口）发起请求
  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr)); //每个字节都用0填充
  serv_addr.sin_family = AF_INET;           //使用IPv4地址
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //具体的IP地址
  serv_addr.sin_port = htons(1234);                   //端口

  char bufSend[BUFSIZ] = {0};
  char bufRecv[BUFSIZ] = {0};

  while (1) {
    //创建套接字
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
      return -1;
    //获取用户输入的字符串并发送给服务器
    printf("Input a string: ");
    fgets(bufSend, BUFSIZ, stdin);

    send(sock, bufSend, strlen(bufSend), 0);
    //接收服务器传回的数据
    recv(sock, bufRecv, BUFSIZ, 0);
    //输出接收到的数据
    printf("Message form server: %s\n", bufRecv);

    memset(bufSend, 0, BUFSIZ); //重置缓冲区
    memset(bufRecv, 0, BUFSIZ); //重置缓冲区

    close(sock);
  }

  return 0;
}