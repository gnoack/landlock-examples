// SPDX-License-Identifier: BSD-3-Clause
/*
 * Landlock sandbox helpers.
 */

#include <err.h>
#include <errno.h>
#include <linux/landlock.h>
#include <linux/prctl.h>
#include <stdlib.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <unistd.h>

#include "landlock_compat.h"

#ifdef LANDLOCK_ACCESS_SOCKET_CREATE

int promise_no_new_sockets() {
  /*
   * Enable "no new privileges" mode (c.f. prctl(2)).
   *
   * This is a prerequisite for enforcing a Landlock ruleset.
   */
  if (prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0) < 0) {
    return -1;
  }

  /*
   * Construct a ruleset with the strongest guarantees we can provide
   * on the currently running kernel, but only up to ABI version 6 and
   * only for socket creation.
   */
  struct landlock_ruleset_attr attr = {
      .handled_access_socket =
          landlock_abi_ruleset_attrs[6].handled_access_socket,
  };
  attr = landlock_min_ruleset_attr(
      &attr, &landlock_abi_ruleset_attrs[landlock_get_abi()]);

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

#else

int promise_no_new_sockets() {
  warn("landlock: not restricting socket(): missing headers");
  return 0; /* missing the Landlock header at compile time */
}

#endif /* LANDLOCK_ACCESS_SOCKET_CREATE */
