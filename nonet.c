/* nonet turns off socket creation and executes a process */

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "sandbox_socket.h"

int main(int argc, char *argv[]) {
  if (promise_no_new_sockets() < 0)
    err(1, "landlock");

  if (argc < 2)
    errx(1, "too few arguments");

  if (execvp(argv[1], argv + 1) < 0)
    err(1, "execve");
}
