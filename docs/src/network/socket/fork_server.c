#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
int main() {

  //创建套接字
  int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  //将套接字和IP、端口绑定
  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr)); //每个字节都用0填充
  serv_addr.sin_family = AF_INET;           //使用IPv4地址
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //具体的IP地址
  serv_addr.sin_port = htons(1234);                   //端口
  int reuse = 1;
  if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) ==
      -1) {
    printf("error!%s", strerror(errno));
    return -1;
  }

  if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
    printf("error!%s", strerror(errno));
    return -1;
  }

  //进入监听状态，等待用户发起请求
  listen(serv_sock, 20);

  //接收客户端请求
  struct sockaddr_in clnt_addr;
  socklen_t clnt_addr_size = sizeof(clnt_addr);
  char buffer[BUFSIZ] = {0};

  while (1) {
    int clnt_sock =
        accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
    //创建子进程 fork返回0则说明位于子进程
    if (!fork()) {
      close(serv_sock); //子进程中关闭主监听套接字

      int strLen = read(clnt_sock, buffer, BUFSIZ); //接收客户端发来的数据
      //向客户端发送数据
      write(clnt_sock, buffer, strLen);
      close(clnt_sock);          //通信结束，关闭通信套接字
      memset(buffer, 0, BUFSIZ); //重置缓冲区
      exit(0); //通信结束 子进程退出，防止进入服务器主循环
    }
  }

  close(serv_sock);

  return 0;
}
