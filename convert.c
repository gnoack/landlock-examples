/* A landlocked version of the UNIX tr command. */

#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "sandbox.h"

void usage() { puts("Usage: convert [-o OUTFILE] [INFILE]"); }

void convert(int infd, int outfd) {
  /*
   * This is a dummy conversion function,
   * which turns all 'x' into 'X'.
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

int main(int argc, char *argv[]) {
  int infd = STDIN_FILENO;
  int outfd = STDOUT_FILENO;
  
  /* Parse flags. */
  int c;
  while ((c = getopt(argc, argv, "o:")) != -1) {
    switch (c) {
    case 'o':
      outfd = creat(optarg, S_IRUSR | S_IWUSR | S_IRGRP);
      if (outfd < 0)
        err(1, "creat");
    }
  }
  if (optind == argc - 1) {
    infd = open(argv[optind], O_RDONLY);
    if (infd < 0)
      err(1, "open");
  } else if (optind != argc) {
    usage();
    err(1, "wrong number of arguments");
  }

  /*
   * Enable the sandbox.
   */
  if (promise_no_further_file_access() < 0)
    err(1, "landlock");

  /* Convert */
  convert(infd, outfd);
}
