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

int main(void)
{
    int count = 5;
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return EXIT_FAILURE;
    }
    if (pid == 0) {
        count = 10;
        printf("Child: count=%d\n", count);
        return EXIT_SUCCESS;
    }
    if (wait_child(pid, NULL) == -1) {
        return EXIT_FAILURE;
    }
    /* The child has exited, but the parent still has its own memory. */
    printf("Parent: count=%d\n", count);
    return EXIT_SUCCESS;
}
