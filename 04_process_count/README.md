# Count processes

1. Before writing code, predict the number of processes after three consecutive `fork()` calls.
2. Perform three `fork()` calls, checking each for errors.
3. After the calls, each process prints one `Hello` line with its PID and PPID.
4. Each process waits for all its direct children using `wait()` or `waitpid()`.

Questions: How many processes exist in total? How many new processes are created? Assuming all calls succeed, generalize the result to `n` consecutive calls made by every process.

Use `n = 3` for this exercise; avoid creating large numbers of processes.

## Build and run

From the project root:

```bash
make -C exercises 04_process_count
./exercises/bin/04_process_count
```

The solution is in `main.c` in this directory, with comments explaining the main steps. Read the exercise and predict the result before running it.
