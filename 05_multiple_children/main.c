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
    pid_t children[3];
    int created = 0;
    int result = EXIT_SUCCESS;
    for (int i = 0; i < 3; ++i) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            result = EXIT_FAILURE;
            break;
        }
        if (pid == 0) {
            printf("Child %d: PID=%ld\n", i + 1, (long)getpid());
            return i + 1; /* The child exits without creating more processes. */
        }
        children[created++] = pid;
    }
    for (int i = 0; i < created; ++i) {
        int status;
        if (wait_child(children[i], &status) == -1) {
            result = EXIT_FAILURE;
            continue;
        }
        if (WIFEXITED(status)) {
            printf("Child PID=%ld exited with status %d\n",
                   (long)children[i], WEXITSTATUS(status));
            if (WEXITSTATUS(status) != i + 1)
                result = EXIT_FAILURE;
        } else {
            fprintf(stderr, "Child PID=%ld terminated abnormally\n", (long)children[i]);
            result = EXIT_FAILURE;
        }
    }
    if (result == EXIT_SUCCESS)
        puts("All children have finished");
    return result;
}
