#include <arpa/inet.h>
#include <err.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void naughty_create_new_socket() {
  printf("*** naughty_create_new_socket()\n");
  int s = socket(AF_INET, SOCK_STREAM, 0);
  if (s < 0) {
    warn("socket(AF_INET, SOCK_STREAM, 0)");
  } else {
    printf("socket() could be created!\n");
    close(s);
  }

  int sv[2];
  if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) < 0)
    warn("socketpair(AF_UNIX, SOCK_STREAM, 0, ...)");
  else {
    printf("socketpair() could be created!\n");
    close(sv[0]);
    close(sv[1]);
  }
}

static int disconnect(int fd) {
  struct sockaddr_in addr = {
      .sin_family = AF_UNSPEC,
  };
  return connect(fd, (struct sockaddr *)&addr, sizeof addr);
}

void naughty_reuse_socket(int client_fd) {
  printf("*** naughty_reuse_socket(%d)\n", client_fd);
  if (disconnect(client_fd) < 0)
    warn("disconnect(%d)", client_fd);

  if (listen(client_fd, 20) < 0)
    warn("listen(%d, 20)", client_fd);
  else
    printf("listen successful\n");

  struct sockaddr_storage peer_addr;
  socklen_t peer_addr_size = sizeof(peer_addr);
  int fd2 = accept(client_fd, (struct sockaddr *)&peer_addr, &peer_addr_size);
  if (fd2 < 0)
    warn("accept");

  write(fd2, "h4x0r\n", 6);
  close(fd2);
}
