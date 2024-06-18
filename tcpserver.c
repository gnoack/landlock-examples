/* tcpserver is a simple TCP server.
 *
 * This example demonstrates how Landlock's socket(2) support
 * interacts with the use of BSD sockets in TCP servers.
 *
 * Differences to a real-world program:
 *
 *  - We play with Landlock's socket creation support here, but a
 *    real-world program should also restrict other types of access
 *    (e.g. to the file system).
 *
 *  - The TCP server is slightly simplified and does not do
 *    multithreading or create subprocesses.  (That would work
 *    though.)
 *
 * Landlock's socket creation support is currently under development
 * (as of June 2024), and the API may change in the future.
 */

#include <err.h>
#include <errno.h>
#include <linux/landlock.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/prctl.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

#include "landlock_compat.h"
#include "naughty.h"

void handle(int fd) {
  /* Pretend that the process got taken over by an attacker. */
  naughty_create_new_socket();
  naughty_reuse_socket(4);

  write(fd, "bye\n", 4);
}

int enable_landlock();

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

  /* bind socket */
  if (bind(sockfd, info->ai_addr, info->ai_addrlen) < 0)
    err(1, "bind");

  /* enable landlock policy */
  if (enable_landlock() < 0)
    err(1, "landlock");

  freeaddrinfo(info);

  /* listen */
  if (listen(sockfd, 20) < 0)
    err(1, "listen");

  /* accept loop */
  for (;;) {
    struct sockaddr_storage peer_addr;
    socklen_t peer_addr_size = sizeof(peer_addr);
    int fd = accept(sockfd, (struct sockaddr *)&peer_addr, &peer_addr_size);
    if (fd < 0)
      err(1, "accept");

    handle(fd);
    close(fd);
  }
}

int enable_landlock() {
  /*
   * Enable "no new privileges" mode (c.f. prctl(2)).
   *
   * This is a prerequisite for enforcing a Landlock ruleset.
   */
  if (prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0) < 0) {
    return -1;
  }

  /*
   * Probe which Landlock version we have available and construct the
   * matching Landlock ruleset attributes, up to ABI version 6.  This
   * means that the following operations will be forbidden, if it is
   * possible:
   *
   *  - Access to file system
   *  - TCP bind(2) and connect(2)
   *  - Creation of new sockets
   */
  struct landlock_ruleset_attr attr;
  if (landlock_get_best_ruleset_attr(&attr, 6) < 0)
    return -1;

  /*
   * Construct a ruleset with the strongest guarantees we can provide
   * at the given ABI version.
   */
  int ruleset_fd =
      syscall(SYS_landlock_create_ruleset, &attr, sizeof(attr), 0U);
  if (ruleset_fd < 0) {
    return -1;
  }

  /*
   * Enforce the ruleset.
   */
  int res = 0;
  if (syscall(SYS_landlock_restrict_self, ruleset_fd, 0) < 0) {
    res = -1;
    goto out;
  }

out:
  close(ruleset_fd);
  return res;
}
