/* tcpserver is a simple TCP server */

#include <err.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "sandbox_socket.h"

void handle(int fd) {
  /*
   * We attempt to open a new socket,
   * but this is going to fail within the Landlock policy.
   */
  int s = socket(AF_INET, SOCK_STREAM, 0);
  if (s < 0) {
    write(fd, "socket() fail\n", 14);
  } else {
    write(fd, "socket() ok\n", 12);
    close(s);
  }
  write(fd, "bye\n", 4);
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    errx(1, "too few arguments; want HOSTNAME PORT");
  }
  char *node = argv[1];    /* hostname */
  char *service = argv[2]; /* port */

  /* look up the address to bind to */
  struct addrinfo hints = {
      .ai_family = AF_UNSPEC,
      .ai_socktype = SOCK_STREAM,
      .ai_flags = AI_PASSIVE,
  };
  struct addrinfo *info;

  int res = getaddrinfo(node, service, &hints, &info);
  if (res != 0) {
    errx(1, "getaddrinfo: %s", gai_strerror(res));
  }

  if (info == NULL) {
    errx(1, "getaddrinfo: Nothing found");
  }

  /* create socket */
  int sockfd = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
  if (sockfd < 0)
    err(1, "socket");

  /* enable landlock policy */
  if (promise_no_new_sockets() < 0)
    err(1, "landlock");

  /* bind socket */
  if (bind(sockfd, info->ai_addr, info->ai_addrlen) < 0)
    err(1, "bind");

  freeaddrinfo(info);

  /* listen */
  if (listen(sockfd, 20) < 0)
    err(1, "listen");

  /* accept loop */
  for (;;) {
    struct sockaddr_storage peer_addr;
    socklen_t peer_addr_size = sizeof(peer_addr);
    int fd = accept(sockfd, (struct sockaddr *)&peer_addr, &peer_addr_size);
    if (fd < 0) {
      err(1, "accept");
    }

    handle(fd);
    close(fd);
  }
}
