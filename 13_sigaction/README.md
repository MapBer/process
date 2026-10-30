# Handle Ctrl+C using sigaction

This example installs a custom `SIGINT` handler. Pressing Ctrl+C normally
terminates the foreground process; here it requests a graceful exit instead.

## Build and run

From the project root:

```bash
make -C exercises 13_sigaction
./exercises/bin/13_sigaction
```

Press Ctrl+C. The program prints a cleanup message and exits successfully.
Alternatively, from another terminal, run `kill -INT <PID>` using the printed PID.

## How it works

1. Block `SIGINT` before installing the handler so an early signal stays pending.
2. Set `sa_handler` to `handle_sigint` and register it with `sigaction()`.
   `sa_mask` specifies additional signals blocked while the handler runs; it is
   empty here. `SIGINT` itself is automatically blocked during its handler.
   `sa_flags = 0` selects the default handler options.
3. The handler sets a `volatile sig_atomic_t` flag. This type supports simple
   flag access between ordinary code and a signal handler.
4. `sigsuspend()` temporarily unblocks `SIGINT` and waits atomically. On handler
   return, it restores the blocked mask and returns `-1` with `errno == EINTR`.
5. The main loop checks the flag, prints the cleanup message, and exits.

Using `if (!stop_requested) pause();` would introduce a race: a signal could
arrive after the check but before `pause()`, leaving the process waiting for
another signal. Blocking the signal during the check and using `sigsuspend()`
avoids that gap.

Do not call `printf()`, `malloc()`, or other functions that are not
async-signal-safe inside the handler. Perform application cleanup in `main()`.
Unlike exercise 10's `sigwait()`, this example runs an asynchronous handler.

Question: Why does the program check the flag in a loop after `sigsuspend()`?

Read local documentation with `man 2 sigaction` and `man 2 sigsuspend`.
