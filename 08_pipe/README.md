# Communicate through a pipe

1. Call `pipe(fd)` before `fork()`; `fd[0]` is the read end and `fd[1]` is the write end.
2. The parent closes the read end, sends `Hello from parent`, and closes the write end.
3. The child closes the write end, reads until EOF, prints the data, and closes the read end.
4. The parent waits for the child using `waitpid()`.
5. Check errors from `pipe()`, `fork()`, `read()`, `write()`, and `waitpid()`.

Expected result: The child prints the message sent by the parent.

Question: Why might reading until EOF block indefinitely if the child keeps its write end open?

Extension: Handle `EINTR` interruptions and partial writes.

Note: `read()` returns a byte count and does not append a null terminator. You can use `fwrite()` to print the bytes read.

## Build and run

From the project root:

```bash
make -C exercises 08_pipe
./exercises/bin/08_pipe
```

The solution is in `main.c` in this directory, with comments explaining the main steps. Read the exercise and predict the result before running it.
