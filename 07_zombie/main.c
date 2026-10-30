#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/* Reap the child process; retry if interrupted by a signal. */
static int wait_child(pid_t pid, int *status)
{
    while (waitpid(pid, status, 0) == -1) {
        if (errno != EINTR) {
            perror("waitpid");
            return -1;
        }
    }
    return 0;
}

static void pause_seconds(unsigned int seconds)
{
    while ((seconds = sleep(seconds)) != 0) {
    }
}

int main(void)
{
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return EXIT_FAILURE;
    }
    if (pid == 0)
        _exit(0);
    printf("Parent PID=%ld, child PID=%ld. Observe state Z during the next 20 seconds.\n",
           (long)getpid(), (long)pid);
    fflush(stdout);
    pause_seconds(20);
    if (wait_child(pid, NULL) == -1)
        return EXIT_FAILURE;
    puts("Child reaped. Verify that its PID has disappeared; waiting another 10 seconds.");
    fflush(stdout);
    pause_seconds(10);
    return EXIT_SUCCESS;
}
