#include <errno.h>
#include <sys/param.h>
#include <sys/syscall.h>
#include <unistd.h>

#include "landlock_compat.h"

/*
 * Landlock ABI compatibility table
 * See https://docs.kernel.org/userspace-api/landlock.html#filesystem-flags
 *
 * The indices in this table are one lower than the ABI version.
 */
struct landlock_ruleset_attr landlock_abi_ruleset_attrs[] = {
    {
        /*
         * ABI v0 does not actually exist, but is sometimes useful to
         * represent configurations where Landlock can not be used.
         */
    },
    {
        /* ABI v1 */
        .handled_access_fs = (1ULL << 13) - 1,
    },
    {
        /* ABI v2: add "refer" */
        .handled_access_fs = (1ULL << 14) - 1,
    },
    {
        /* ABI v3: add "truncate" */
        .handled_access_fs = (1ULL << 15) - 1,
    },
    {
        /* ABI v4: add TCP bind and connect */
        .handled_access_fs = (1ULL << 15) - 1,
        .handled_access_net = (1ULL << 2) - 1,
    },
    {
        /* ABI v5: add "ioctl_dev" */
        .handled_access_fs = (1ULL << 16) - 1,
        .handled_access_net = (1ULL << 2) - 1,
    },
#ifdef LANDLOCK_ACCESS_SOCKET_CREATE
    {
        /* ABI v6: add socket creation */
        .handled_access_fs = (1ULL << 16) - 1,
        .handled_access_net = (1ULL << 2) - 1,
        .handled_access_socket = (1ULL << 1) - 1,
    },
#endif /* LANDLOCK_ACCESS_SOCKET_CREATE */
};

#define ARRAY_SIZE(A) (sizeof(A) / sizeof(A[0]))

int landlock_get_abi() {
  /*
   * Probe for the available Landlock ABI version using
   * landlock_create_ruleset(2).
   */
  int abi = syscall(SYS_landlock_create_ruleset, NULL, 0,
                    LANDLOCK_CREATE_RULESET_VERSION);
  if (abi < 0) {
    switch (errno) {
    case ENOSYS:     /* Landlock unsupported */
    case EOPNOTSUPP: /* Landlock disabled */
      return 0;
    }
    /* Unknown other error, should never happen */
    return 0;
  }

  if (abi > ARRAY_SIZE(landlock_abi_ruleset_attrs) - 1) {
    /*
     * The kernel supports features that we don't know yet:
     * Treat it as the highest known Landlock version.
     */
    abi = ARRAY_SIZE(landlock_abi_ruleset_attrs) - 1;
  }
  return abi;
}

struct landlock_ruleset_attr
landlock_min_ruleset_attr(struct landlock_ruleset_attr *a,
                          struct landlock_ruleset_attr *b) {
  struct landlock_ruleset_attr res = {
      .handled_access_fs = a->handled_access_fs & b->handled_access_fs,
      .handled_access_net = a->handled_access_net & b->handled_access_net,
#ifdef LANDLOCK_ACCESS_SOCKET_CREATE
      .handled_access_socket =
          a->handled_access_socket & b->handled_access_socket,
#endif /* LANDLOCK_ACCESS_SOCKET_CREATE */
  };
  return res;
}
