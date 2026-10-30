#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void)
{
    int result = EXIT_SUCCESS;
    /* Every process continues the loop: 1 -> 2 -> 4 -> 8. */
    for (int i = 0; i < 3; ++i) {
        if (fork() == -1) {
            perror("fork");
            result = EXIT_FAILURE;
            break;
        }
    }
    printf("Hello: PID=%ld, PPID=%ld\n", (long)getpid(), (long)getppid());
    /* Each process waits only for its own direct children. */
    for (;;) {
        int status;
        if (waitpid(-1, &status, 0) > 0) {
            if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
                result = EXIT_FAILURE;
            continue;
        }
        if (errno == EINTR)
            continue;
        if (errno != ECHILD) {
            perror("waitpid");
            result = EXIT_FAILURE;
        }
        break;
    }
    return result;
}
