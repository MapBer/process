# Notify a child using a signal

1. Block `SIGUSR1` using `sigprocmask()` before `fork()`.
2. The child inherits the signal mask and waits with `sigwait()`.
3. The parent sends `SIGUSR1` to the child using `kill()`.
4. The child consumes the signal, prints a message, and exits.
5. The parent reaps the child with `waitpid()` and restores its signal mask.

Despite its name, `kill()` can send a notification without terminating a process.
The default action for `SIGUSR1` is termination, so blocking it before sending
and consuming it with `sigwait()` matters. An early signal remains pending;
no `sleep()` is needed to make the child ready.

## Build and run

From the project root:

```bash
make -C exercises 10_signal
./exercises/bin/10_signal
```

Expected output: `Child received SIGUSR1 (...)`, with the platform's signal number.

Question: What race could occur if you used `pause()` after checking a flag?

Signals are notifications, not byte streams. Repeated standard signals can
coalesce into a single pending signal. For an asynchronous handler example,
explore `sigaction()`: handlers must use async-signal-safe operations, so do not
call `printf()` from a handler. Printing after `sigwait()` is ordinary program code.

Read local documentation with `man 2 kill`, `man 2 sigprocmask`, and `man 3 sigwait`.
