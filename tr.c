/* A landlocked version of the UNIX tr command.
 *
 * This example program is a simplified version of the UNIX `tr`
 * utility.
 *
 * The utility reads from stdin and writes to stdout:
 * It expects two strings as input and replaces the bytes in the
 * first string with the matching bytes in the second string:
 *
 * $ echo baneni | tr aeiou aaaaa
 * banana
 *
 * The Landlock sandbox we use here has the following properties:
 *
 * - It disables all further file accesses which are restrictable with
 *   Landlock.
 * - It is backwards compatible with older Linux kernels.
 *
 * This is suitable for `tr`, because:
 *
 * - All inputs and outputs are already opened (stdin and stdout)
 *   and no further files are needed.
 * - A compiled binary for the utility might be run on various Linux
 *   installations and should be compatible with older kernels.
 */

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "sandbox_file.h"

void usage() { puts("Usage: tr STRING1 STRING2"); }

int main(int argc, char *argv[]) {
  /*
   * Enable the sandbox.
   */
  if (promise_no_further_file_access() < 0)
    err(1, "landlock");

  /*
   * Parse command line arguments into a translation table.
   */
  if (argc != 3) {
    usage();
    return 0;
  }

  char map[256];
  for (int i = 0; i < 256; i++) {
    map[i] = i;
  }
  for (char *s = argv[1], *r = argv[2]; *s; s++, r++) {
    if (!*r)
      errx(1, "too short replacement string");

    map[(int)*s] = *r;
  }

  /*
   * Process stdin through the translation table and write the results
   * to stdout.  (Yes, this should be buffered, but that is not the
   * point of this example.)
   */
  char c;
  while (1) {
    int ret = read(0, &c, 1);
    if (ret < 0) {
      err(1, "read");
    } else if (ret == 0) {
      return 0; /* EOF */
    }
    c = map[(int)c];
    if (write(1, &c, 1) < 0) {
      err(1, "write");
    }
  }
}
