/*
 * promise_no_further_file_access - Enable Landlock and restrict all file
 * accesses.
 *
 * After this function returns, no further files can be opened through
 * their file paths with open(2), and directories can not be listed or
 * modified.
 *
 * Previously opened file descriptors keep working.
 *
 * This function is suitable for processing tools in Unix pipelines.
 *
 * Errors:
 *   Returns 0 on success, 1 on error.
 *   Populates errno with an error code from Landlock.
 */
int promise_no_further_file_access();

