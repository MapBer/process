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
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return EXIT_FAILURE;
    }
    if (pid == 0) {
        printf("Child: PID=%ld, PPID=%ld\n",
               (long)getpid(), (long)getppid());
        return EXIT_SUCCESS;
    }
    printf("Parent: PID=%ld, child PID=%ld\n", (long)getpid(), (long)pid);
    /* The two messages may appear in either order. */
    return wait_child(pid, NULL) == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
