#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SERVER_PORT 80
#define SERVER_NAME "www.google.com"
#define GET_REQUEST "GET / HTTP/1.0\r\n\r\n"

int main(void) {
  int ret, len, server_fd;
  unsigned char buf[1024];
  struct sockaddr_in server_addr;
  struct hostent *server_host;

  /*
   * Start the connection
   */
  printf("\n  . Connecting to tcp/%s/%4d...", SERVER_NAME, SERVER_PORT);
  fflush(stdout);

  if ((server_host = gethostbyname(SERVER_NAME)) == NULL) {
    printf(" failed\n  ! gethostbyname failed\n\n");
    goto exit;
  }

  if ((server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) < 0) {
    printf(" failed\n  ! socket returned %d\n\n", server_fd);
    goto exit;
  }

  memcpy((void *)&server_addr.sin_addr, (void *)server_host->h_addr,
         server_host->h_length);

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(SERVER_PORT);

  if ((ret = connect(server_fd, (struct sockaddr *)&server_addr,
                     sizeof(server_addr))) < 0) {
    printf(" failed\n  ! connect returned %d\n\n", ret);
    goto exit;
  }

  printf(" ok\n");

  /*
   * Write the GET request
   */
  printf("  > Write to server:");
  fflush(stdout);

  len = sprintf((char *)buf, GET_REQUEST);

  while ((ret = write(server_fd, buf, len)) <= 0) {
    if (ret != 0) {
      printf(" failed\n  ! write returned %d\n\n", ret);
      goto exit;
    }
  }

  len = ret;
  printf(" %d bytes written\n\n%s", len, (char *)buf);

  /*
   * Read the HTTP response
   */
  printf("  < Read from server:");
  fflush(stdout);
  do {
    len = sizeof(buf) - 1;
    memset(buf, 0, sizeof(buf));
    ret = read(server_fd, buf, len);

    if (ret <= 0) {
      printf("failed\n  ! ssl_read returned %d\n\n", ret);
      break;
    }

    len = ret;
    printf(" %d bytes read\n\n%s", len, (char *)buf);
  } while (1);

exit:

  close(server_fd);
  return (ret);
}