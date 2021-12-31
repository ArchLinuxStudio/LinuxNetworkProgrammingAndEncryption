#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
int main() {
  struct hostent *host = gethostbyname("www.baidu.com");
  if (!host) {
    puts("Get IP address error!");
    return -1;
  }
  //别名
  for (int i = 0; host->h_aliases[i]; i++) {
    printf("Aliases %d: %s\n", i + 1, host->h_aliases[i]);
  }
  //地址类型
  printf("Address type: %s\n",
         (host->h_addrtype == AF_INET) ? "AF_INET" : "AF_INET6");
  // IP地址
  for (int i = 0; host->h_addr_list[i]; i++) {
    printf("IP addr %d: %s\n", i + 1,
           inet_ntoa(*(struct in_addr *)host->h_addr_list[i]));
  }
  puts(host->h_name);
  printf("%d\n", host->h_length);
  return 0;
}