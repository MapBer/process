# Run a command using exec

1. The parent calls `fork()`.
2. The child calls `execlp("ls", "ls", "-l", (char *)NULL)`.
3. If exec fails, the child calls `perror()` and `_exit(127)`.
4. The parent calls `waitpid()`, checks the status, and prints a completion message.
5. Try a nonexistent command to exercise the error path:

```bash
./exercises/bin/06_exec nonexistent_command
```

Questions: Does a successful exec execute the statement immediately after it? Does the PID change?

## Build and run

From the project root:

```bash
make -C exercises 06_exec
./exercises/bin/06_exec
```

The solution is in `main.c` in this directory, with comments explaining the main steps. Read the exercise and predict the result before running it.
