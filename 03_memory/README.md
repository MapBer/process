# Separate memory in parent and child processes

1. Declare `int count = 5` before calling `fork()`.
2. In the child, change `count` to 10, print it, and exit.
3. In the parent, wait for the child using `waitpid()`, then print `count`.

Expected result: The child prints 10, and the parent prints 5.

Question: Why does `waitpid()` not give the parent the child's updated value of `count`?

## Build and run

From the project root:

```bash
make -C exercises 03_memory
./exercises/bin/03_memory
```

The solution is in `main.c` in this directory, with comments explaining the main steps. Read the exercise and predict the result before running it.
