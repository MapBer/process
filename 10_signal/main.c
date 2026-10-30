#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void)
{
    sigset_t signals, previous;
    sigemptyset(&signals);
    sigaddset(&signals, SIGUSR1);
    /* Block before fork: an early signal stays pending until sigwait(). */
    if (sigprocmask(SIG_BLOCK, &signals, &previous) == -1) {
        perror("sigprocmask");
        return EXIT_FAILURE;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return EXIT_FAILURE;
    }
    if (pid == 0) {
        int received;
        /* sigwait consumes the blocked signal; no signal handler is needed. */
        int error = sigwait(&signals, &received);
        if (error != 0) {
            fprintf(stderr, "sigwait: %s\n", strerror(error));
            return EXIT_FAILURE;
        }
        if (printf("Child received SIGUSR1 (%d)\n", received) < 0 ||
            fflush(stdout) == EOF)
            return EXIT_FAILURE;
        return EXIT_SUCCESS;
    }

    int result = EXIT_SUCCESS;
    if (kill(pid, SIGUSR1) == -1) {
        perror("kill");
        /* Avoid leaving a child waiting if sending the notification fails. */
        if (kill(pid, SIGKILL) == -1 && errno != ESRCH)
            perror("kill SIGKILL");
        result = EXIT_FAILURE;
    }
    int status;
    while (waitpid(pid, &status, 0) == -1) {
        if (errno == EINTR)
            continue;
        perror("waitpid");
        return EXIT_FAILURE;
    }
    if (!WIFEXITED(status) || WEXITSTATUS(status) != EXIT_SUCCESS)
        result = EXIT_FAILURE;
    if (sigprocmask(SIG_SETMASK, &previous, NULL) == -1) {
        perror("sigprocmask restore");
        result = EXIT_FAILURE;
    }
    return result;
}
