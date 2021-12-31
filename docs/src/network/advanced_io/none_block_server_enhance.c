#include <arpa/inet.h>
#include <asm-generic/errno.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
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
  int round = 0;

  //记录需要监视客户端socket 后续需要轮询
  typedef struct readNode {
    int fd;
    struct readNode *next;
  } readNode;

  //初始化 相当于头指针 一个空头链表
  readNode *readList = (readNode *)malloc(sizeof(readNode));

  readNode *currentNode; //用作尾指针
  currentNode = readList;

  //工作指针
  readNode *pre = readList;
  readNode *worker = pre->next;

  //每次循环先检查是否有可以接受的新链接
  //随后再检查已有客户端链接列表中是否有可读事件
  while (1) {
    errno = 0;
    int clnt_sock =
        accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);

    //如果尚无链接 stderr会被设置
    //此时是非阻塞的情形，可判断继续循环，也可以执行一些别的任务或暂停操作
    printf("clnt_sock: %d\n", clnt_sock);
    if (clnt_sock == -1) {
      // fprintf(stderr, "errno = %d, err: %s\n", errno, strerror(errno));
      close(clnt_sock);
      //暂时无链接，可以先睡眠一阵
      sleep(1);
    }

    //如有连接到达，加入链表中
    if (clnt_sock > 0) {
      puts("clnt_sock received");
      //如accept成功，对需要使用的客户端套接字设置 O_NONBLOCK
      fcntl(clnt_sock, F_SETFL, fcntl(clnt_sock, F_GETFL, 0) | O_NONBLOCK);
      //新建结构并将客户端套接字结构加入链表
      readNode *newNode = (readNode *)malloc(sizeof(readNode));
      newNode->fd = clnt_sock;
      newNode->next = NULL;
      currentNode->next = newNode;
      currentNode = newNode;
    }

    //重置工作指针
    pre = readList;
    worker = pre->next;
    // readList链表中有fd的话需要轮询
    for (; pre->next != NULL; pre = pre->next, worker = pre->next) {
      errno = 0;
      //非头节点， 查询worker
      int strLen = read(worker->fd, buffer, BUFSIZ); //接收客户端发来的数据
      printf("error code: %d\n", errno);
      if (errno == EWOULDBLOCK) {
        continue;
      } else if (strLen <= 0) {
        //关闭套接字
        close(worker->fd);
        printf("client(eventfd=%d) disconnected.\n", worker->fd);
        memset(buffer, 0, BUFSIZ); //重置缓冲区

        //移除此节点
        if (worker->next) {
          // worker不为最后一个节点
          pre->next = worker->next;
          free(worker);
          worker = pre->next;
        } else {
          // worker为最后一个节点
          pre->next = NULL;
          free(worker);
          worker = pre->next;
          currentNode = pre;
          break;
        }
      } else {
        int writeLen = write(worker->fd, buffer, strLen);
      }
    }
  }
  close(serv_sock);

  return 0;
}
