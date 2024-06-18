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

/*
 * Compatibility table for file system access rights.
 * See https://docs.kernel.org/userspace-api/landlock.html#filesystem-flags
 *
 * (The indices in this table are one lower than the ABI version.)
 */
static __u64 landlock_fs_access_rights[] = {
    (1ULL << 13) - 1, /* ABI v1                 */
    (1ULL << 14) - 1, /* ABI v2: add "refer"    */
    (1ULL << 15) - 1, /* ABI v3: add "truncate" */
};

#define ARRAY_SIZE(A) (sizeof(A) / sizeof(A[0]))

int promise_no_further_file_access() {
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

  if (abi > ARRAY_SIZE(landlock_fs_access_rights)) {
    /*
     * The kernel supports features that we don't know yet:
     * Treat it as the highest known Landlock version.
     */
    abi = ARRAY_SIZE(landlock_fs_access_rights);
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
      .handled_access_fs = landlock_fs_access_rights[abi - 1],
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
