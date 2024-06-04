/*
 * promise_no_new_sockets - Disable creation of new sockets.
 *
 * Previously created socket file descriptors keep working.
 *
 * Returns 0 on success, 1 on error.
 * Populates errno with an error code from Landlock.
 */
int promise_no_new_sockets();
