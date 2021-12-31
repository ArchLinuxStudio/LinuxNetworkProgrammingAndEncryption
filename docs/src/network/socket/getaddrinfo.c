#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
  struct addrinfo *res;
  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = PF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  if (getaddrinfo("www.gnu.org", "80", &hints, &res) == -1)
    return -1;
  int s = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

  if (connect(s, res->ai_addr, res->ai_addrlen) == -1)
    return -1;

  // read...etc
  // ...
  // ...
  close(s);
  freeaddrinfo(res);

  return 0;
}
