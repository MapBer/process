# Communicate through shared memory

Use System V shared memory for interprocess communication (IPC) on Linux.
Compare this exercise with exercise 3, where changes to an ordinary variable
in the child do not affect the parent's copy.

1. Create a segment with `shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0600)`.
2. Attach it to the parent's address space using `shmat()` and initialize the shared integer to 10.
3. Mark the segment for deletion with `shmctl(..., IPC_RMID, ...)`. The attachment remains valid until detached; the kernel removes the segment after its last attachment is gone.
4. Call `fork()`. The child inherits the shared memory attachment and adds 5 to the integer.
5. The child detaches using `shmdt()` and exits.
6. The parent waits using `waitpid()`, prints the updated value, and detaches.

Expected output:

```text
Parent sees shared count = 15
```

`IPC_PRIVATE` requests a new segment; it does not itself set access permissions.
The `0600` mode grants read and write access only to the owner. Unlike `shmat()`,
which returns `(void *)-1` on failure, `shmget()` returns an integer ID or -1.

Shared memory transfers data without `read()` or `write()`, but it does not
provide synchronization. Here the parent reads only after the child terminates.
If both processes repeatedly updated the value at the same time, they would
need synchronization, such as a process-shared semaphore. `volatile` would not
make concurrent increments safe.

Question: Why does the parent see 15 here, while an ordinary local variable
would remain 10 after the child modifies its own copy?

Extension: Store a structure containing an integer and a message in the segment.
Have the child fill it and the parent print it after waiting.

## Build and run

From the project root:

```bash
make -C exercises 09_shared_memory
./exercises/bin/09_shared_memory
```

No extra libraries are required. The solution is in `main.c` in this directory.
Read local documentation with `man 2 shmget`, `man 2 shmat`, and `man 2 shmctl`.
