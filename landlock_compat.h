#include <linux/landlock.h>

extern struct landlock_ruleset_attr landlock_abi_ruleset_attrs[];

/*
 * landlock_get_abi - determine the newest known(!) Landlock ABI
 * version which can be used on the currently running Linux kernel.
 *
 * The result is guaranteed to be a valid index of
 * landlock_abi_ruleset_attrs.
 *
 * This function does not return an error.
 */
int landlock_get_abi();

/*
 * landlock_min_ruleset_attr - determine the smallest common
 * denominator of the two ruleset attributes a and b.
 *
 * This function does not return an error, but may return an empty
 * ruleset.
 */
struct landlock_ruleset_attr
landlock_min_ruleset_attr(struct landlock_ruleset_attr *a,
                          struct landlock_ruleset_attr *b);
