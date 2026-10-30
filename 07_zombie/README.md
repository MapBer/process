# Observe a zombie process

1. The parent creates a child, which exits immediately.
2. The parent prints the child's PID and sleeps for 20 seconds before calling `waitpid()`.
3. In another terminal, use `ps -eo pid,ppid,stat,comm` to find the child in state `Z`.
4. The parent calls `waitpid()`, prints a message confirming that the child was reaped, and sleeps for another 10 seconds.
5. Use `ps` to verify that the child's PID has disappeared.

Questions: Does a zombie still execute code? What information does `waitpid()` allow the system to release?

Create only one child and always reap it after the observation period.

## Build and run

From the project root:

```bash
make -C exercises 07_zombie
./exercises/bin/07_zombie
```

The solution is in `main.c` in this directory, with comments explaining the main steps. Read the exercise and predict the result before running it.
