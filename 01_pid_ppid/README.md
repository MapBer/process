# Observe PID and PPID

1. Print the PID using `getpid()` and the PPID using `getppid()`.
2. Keep the program running for 20 seconds using `sleep(20)`.
3. Open another terminal and inspect it with `ps -eo pid,ppid,stat,comm`.

Questions: Which program does the PPID identify? Does the PID change between runs?

## Build and run

From the project root:

```bash
make -C exercises 01_pid_ppid
./exercises/bin/01_pid_ppid
```

The solution is in `main.c` in this directory, with comments explaining the main steps. Read the exercise and predict the result before running it.
