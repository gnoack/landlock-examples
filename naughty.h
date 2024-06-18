/*
 * A collection of things that an attacker might do to attempt
 * breaking out of a sandbox.
 */

void naughty_create_new_socket();

void naughty_reuse_socket(int fd);
