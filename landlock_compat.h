#include <linux/landlock.h>

/*
 * get_best_landlock_ruleset_attr - determine the strongest Landlock
 * ruleset which can be enforced on the currently running Linux
 * kernel and under the given maximum ABI version.
 *
 * Populates the *attr struct.
 *
 * Returns  0 on success.
 * Returns -1 on error, and sets errno.
 * Returns -2 if Landlock is not available, and sets *attr to zero.
 */
int landlock_get_best_ruleset_attr(struct landlock_ruleset_attr *attr,
                                   int max_abi);
