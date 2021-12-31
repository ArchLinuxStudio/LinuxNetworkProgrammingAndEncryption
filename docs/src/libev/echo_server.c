#include <errno.h>
#include <ev.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT_NO 1234
#define BUFFER_SIZE 1024

int total_clients = 0; // Total number of connected clients

void accept_cb(struct ev_loop *loop, struct ev_io *watcher, int revents);
void read_cb(struct ev_loop *loop, struct ev_io *watcher, int revents);

int main() {
  struct ev_loop *loop = ev_default_loop(0);
  int sd;
  struct sockaddr_in addr;

  // Create server socket
  if ((sd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket error");
    return -1;
  }

  // is nonblock needed?
  // int flags = fcntl(sd, F_GETFL, 0);
  // fcntl(sd, F_SETFL, flags | O_NONBLOCK);

  int reuse = 1;
  if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) == -1) {
    perror("setsockopt error");
    return -1;
  }

  bzero(&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT_NO);
  addr.sin_addr.s_addr = INADDR_ANY;

  // Bind socket to address
  if (bind(sd, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
    perror("bind error");
  }

  // Start listing on the socket
  if (listen(sd, SOMAXCONN) < 0) {
    perror("listen error");
    return -1;
  }

  struct ev_io w_accept;
  // Initialize and start a watcher to accepts client requests
  ev_io_init(&w_accept, accept_cb, sd, EV_READ);
  ev_io_start(loop, &w_accept);

  // Start infinite loop
  // this loop may not needed
  // while (1) {
  ev_loop(loop, 0);
  // }

  return 0;
}

/* Accept client requests */
void accept_cb(struct ev_loop *loop, struct ev_io *watcher, int revents) {
  struct sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);
  int client_sd;
  struct ev_io *w_client = (struct ev_io *)malloc(sizeof(struct ev_io));

  if (EV_ERROR & revents) {
    perror("got invalid event");
    return;
  }

  // Accept client request
  client_sd = accept(watcher->fd, (struct sockaddr *)&client_addr, &client_len);

  if (client_sd < 0) {
    perror("accept error");
    return;
  }

  total_clients++; // Increment total_clients count
  printf("Successfully connected with client.\n");
  printf("%d client(s) connected.\n", total_clients);

  // Initialize and start watcher to read client requests
  ev_io_init(w_client, read_cb, client_sd, EV_READ);
  ev_io_start(loop, w_client);
}

/* Read client message */
void read_cb(struct ev_loop *loop, struct ev_io *watcher, int revents) {
  char buffer[BUFFER_SIZE];
  ssize_t read;

  if (EV_ERROR & revents) {
    perror("got invalid event");
    return;
  }

  // Receive message from client socket
  read = recv(watcher->fd, buffer, BUFFER_SIZE, 0);

  if (read < 0) {
    // perror("read error");
    printf("read error: %s", strerror(errno));
    return;
  }

  if (read == 0) {
    // Stop and free watcher if client socket is closing
    ev_io_stop(loop, watcher);
    close(watcher->fd);
    free(watcher);
    perror("peer might closing");
    total_clients--; // Decrement total_clients count
    printf("%d client(s) connected.\n", total_clients);
    return;
  } else {
    printf("message:%s\n", buffer);
  }

  // Send message bach to the client
  send(watcher->fd, buffer, read, 0);
  bzero(buffer, read);
}