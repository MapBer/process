#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static volatile sig_atomic_t stop_requested = 0;

static void handle_sigint(int signal_number)
{
    (void)signal_number;
    /* Keep the handler minimal: printf() is not async-signal-safe. */
    stop_requested = 1;
}

int main(void)
{
    sigset_t blocked, previous, waiting;
    sigemptyset(&blocked);
    sigaddset(&blocked, SIGINT);
    if (sigprocmask(SIG_BLOCK, &blocked, &previous) == -1) {
        perror("sigprocmask");
        return EXIT_FAILURE;
    }

    struct sigaction action = {0};
    action.sa_handler = handle_sigint;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    if (sigaction(SIGINT, &action, NULL) == -1) {
        perror("sigaction");
        return EXIT_FAILURE;
    }

    printf("PID: %ld. Press Ctrl+C to request a graceful exit.\n", (long)getpid());
    fflush(stdout);

    waiting = previous;
    sigdelset(&waiting, SIGINT);
    while (!stop_requested) {
        /* Atomically unblock SIGINT and wait: no check/pause race. */
        if (sigsuspend(&waiting) == -1 && errno != EINTR) {
            perror("sigsuspend");
            return EXIT_FAILURE;
        }
    }

    /* Ordinary code can print and perform application cleanup safely. */
    puts("SIGINT received. Cleaning up and exiting normally.");
    if (sigprocmask(SIG_SETMASK, &previous, NULL) == -1) {
        perror("sigprocmask restore");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
