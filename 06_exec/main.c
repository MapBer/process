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

int main(int argc, char *argv[])
{
    /* Pass a nonexistent command to test error handling: ./06_exec nonexistent_command */
    const char *command = argc > 1 ? argv[1] : "ls";
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return EXIT_FAILURE;
    }
    if (pid == 0) {
        execlp(command, command, "-l", (char *)NULL);
        perror("execlp");
        _exit(127); /* A successful exec never returns here. */
    }
    int status;
    if (wait_child(pid, &status) == -1)
        return EXIT_FAILURE;
    if (!WIFEXITED(status)) {
        fputs("Command terminated abnormally\n", stderr);
        return EXIT_FAILURE;
    }
    printf("Command finished with exit status: %d\n", WEXITSTATUS(status));
    return WEXITSTATUS(status);
}
