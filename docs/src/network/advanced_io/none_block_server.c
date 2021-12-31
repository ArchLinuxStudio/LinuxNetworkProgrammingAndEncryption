#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {

  //创建套接字
  int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  //对服务端套接字设置  O_NONBLOCK
  fcntl(serv_sock, F_SETFL, fcntl(serv_sock, F_GETFL, 0) | O_NONBLOCK);

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
    errno = 0;
    int clnt_sock =
        accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);

    //如果尚无链接 stderr会被设置
    //此时是非阻塞的情形，可判断继续循环，也可以执行一些别的任务或暂停操作
    printf("%d\n", clnt_sock);
    if (clnt_sock == -1) {
      fprintf(stderr, "errno = %d, err: %s\n", errno, strerror(errno));
      close(clnt_sock);
      //暂时无链接，可以先睡眠一阵
      sleep(1);
      continue;
    }

    //如accept成功，对需要使用的客户端套接字设置 O_NONBLOCK
    fcntl(clnt_sock, F_SETFL, fcntl(clnt_sock, F_GETFL, 0) | O_NONBLOCK);

    int strLen = read(clnt_sock, buffer, BUFSIZ); //接收客户端发来的数据
    //非阻塞的读，-1说明没读到
    while (strLen == -1) {
      //此处打印说明为非阻塞
      printf("Our server is none blocked,You can do something when waiting for "
             "client data.\n");
      strLen = read(clnt_sock, buffer, BUFSIZ);
      sleep(1);
    }

    //向客户端发送数据
    int writeLen = write(clnt_sock, buffer, strLen);
    while (writeLen == -1) {
      printf("Our server is none blocked,You can do something when waiting "
             "client to receive data.\n");
      strLen = write(clnt_sock, buffer, strLen);
      sleep(1);
    }
    //关闭套接字
    close(clnt_sock);
    memset(buffer, 0, BUFSIZ); //重置缓冲区
  }

  close(serv_sock);

  return 0;
}
