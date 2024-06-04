// SPDX-License-Identifier: BSD-3-Clause
/*
 * Landlock sandbox helpers.
 */

#include <errno.h>
#include <linux/landlock.h>
#include <linux/prctl.h>
#include <stdlib.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <unistd.h>

#ifdef LANDLOCK_ACCESS_SOCKET_CREATE

int promise_no_new_sockets() {
  /*
   * Probe for the available Landlock ABI version using
   * landlock_create_ruleset(2).
   *
   * This might return errors if Landlock is not supported by the
   * kernel or if it is disabled at boot time.  We want to use the
   * strongest available guarantees in a "best effort" way, so we
   * return no error in this case.
   */
  int abi = syscall(SYS_landlock_create_ruleset, NULL, 0,
                    LANDLOCK_CREATE_RULESET_VERSION);
  if (abi < 0) {
    switch (errno) {
    case ENOSYS:     /* Landlock unsupported */
    case EOPNOTSUPP: /* Landlock disabled */
      return 0;
    }
    return -1;
  }

  if (abi < 6) {
    /*
     * Landlock socket creation support is only available since
     * version 6 of the Landlock ABI.  On earlier ABI versions,
     * we can't do anything, so we degrade gracefully.
     */
    return 0;
  }

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
   * at the given ABI version.
   */
  struct landlock_ruleset_attr attr = {
    .handled_access_socket = LANDLOCK_ACCESS_SOCKET_CREATE,
  };
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
  return 0;  /* missing the Landlock header at compile time */
}

#endif  /* LANDLOCK_ACCESS_SOCKET_CREATE */
