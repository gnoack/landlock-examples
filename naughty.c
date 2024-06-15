#include <err.h>
#include <stdio.h>
#include <sys/socket.h>
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
