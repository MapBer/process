# Create a child process

1. Call `fork()` once and check for errors.
2. In the child, print a message identifying it as the child, along with its PID and PPID.
3. In the parent, print a message identifying it as the parent, along with its PID and the child's PID.
4. After printing, the parent calls `waitpid()` to reap the child.
5. Run the program several times and observe the order of the messages.

Question: Why is the parent not guaranteed to print first?

## Build and run

From the project root:

```bash
make -C exercises 02_fork
./exercises/bin/02_fork
```

The solution is in `main.c` in this directory, with comments explaining the main steps. Read the exercise and predict the result before running it.
