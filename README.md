# Simple Landlock sandboxing example in C (`tr`)

This example program is a simplified version of the UNIX `tr` utility.

The utility reads from stdin and writes to stdout:
It expects two strings as input and replaces the bytes in the
first string with the matching bytes in the second string:

```
$ echo baneni | tr aeiou aaaaa
banana
```

The Landlock sandbox we use here has the following properties:

* It disables all further file accesses which are restrictable with Landlock.
* It is backwards compatible with older Linux kernels.

This is suitable for `tr`, because:

* All inputs and outputs are already opened (stdin and stdout)
  and no further files are needed.
* A compiled binary for the utility might be run on various Linux
  installations and should be compatible with older kernels.
