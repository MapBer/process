# Create and wait for multiple children

1. In the parent, use a loop to create exactly three children and store their PIDs.
2. Each child prints its index and PID, then returns immediately to avoid creating more children.
3. Child `i` exits with status `i + 1`, where indexing starts at zero.
4. The parent uses `waitpid()` to wait for every child successfully created.
5. Check `WIFEXITED()` before using `WEXITSTATUS()` to print the exit status.
6. Print a message confirming that all children have finished after reaping them successfully.

Question: Does waiting for PIDs in creation order guarantee the order in which children execute?

If `fork()` fails partway through the loop, the parent must still reap the children already created.

## Build and run

From the project root:

```bash
make -C exercises 05_multiple_children
./exercises/bin/05_multiple_children
```

The solution is in `main.c` in this directory, with comments explaining the main steps. Read the exercise and predict the result before running it.
