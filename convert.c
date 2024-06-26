/* A landlocked version of a file conversion command.
 *
 * More documentation at
 * https://wiki.gnoack.org/LandlockConvertExample
 */

#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "sandbox_file.h"

void usage() { puts("Usage: convert [-o OUTFILE] [INFILE]"); }

void convert(int infd, int outfd) {
  /*
   * This is a dummy conversion function,
   * which turns all 'x' into 'X'.
   * In a real-life scenario, this conversion would be more involved
   * and would pose a big attack surface.
   */
  char c;
  while (1) {
    int ret = read(infd, &c, 1);
    if (ret < 0)
      err(1, "read");
    else if (ret == 0)
      return; /* EOF */

    if (c == 'x') {
      c = 'X';
    }

    if (write(outfd, &c, 1) < 0)
      err(1, "write");
  }
}

void parse_flags(int argc, char *argv[], int *infd, int *outfd) {
  int c;
  while ((c = getopt(argc, argv, "o:")) != -1) {
    switch (c) {
    case 'o':
      *outfd = creat(optarg, S_IRUSR | S_IWUSR | S_IRGRP);
      if (*outfd < 0)
        err(1, "creat");
    }
  }
  if (optind == argc - 1) {
    *infd = open(argv[optind], O_RDONLY);
    if (*infd < 0)
      err(1, "open");
  } else if (optind != argc) {
    usage();
    errx(1, "wrong number of arguments");
  }
}

int main(int argc, char *argv[]) {
  /* Initialization phase: Parse flags. */
  int infd = STDIN_FILENO;
  int outfd = STDOUT_FILENO;
  parse_flags(argc, argv, &infd, &outfd);

  /* Enable the sandbox. */
  if (promise_no_further_file_access() < 0)
    err(1, "landlock");

  /* Start processing untrusted input: Convert. */
  convert(infd, outfd);
}
