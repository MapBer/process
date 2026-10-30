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
    int fd[2];
    if (pipe(fd) == -1) {
        perror("pipe");
        return EXIT_FAILURE;
    }
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        close(fd[0]);
        close(fd[1]);
        return EXIT_FAILURE;
    }
    if (pid == 0) {
        close(fd[1]); /* Close the write end so this reader can eventually receive EOF. */
        char buffer[64];
        for (;;) {
            ssize_t count = read(fd[0], buffer, sizeof(buffer));
            if (count == 0)
                break;
            if (count == -1) {
                if (errno == EINTR)
                    continue;
                perror("read");
                close(fd[0]);
                return EXIT_FAILURE;
            }
            /* read() does not append a null terminator ('\0'). */
            if (fwrite(buffer, 1, (size_t)count, stdout) != (size_t)count) {
                perror("fwrite");
                close(fd[0]);
                return EXIT_FAILURE;
            }
        }
        close(fd[0]);
        return fflush(stdout) == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
    }
    close(fd[0]);
    const char message[] = "Hello from parent\n";
    size_t sent = 0;
    int result = EXIT_SUCCESS;
    while (sent < sizeof(message) - 1) {
        ssize_t count = write(fd[1], message + sent, sizeof(message) - 1 - sent);
        if (count == -1) {
            if (errno == EINTR)
                continue;
            perror("write");
            result = EXIT_FAILURE;
            break;
        }
        if (count == 0) {
            fputs("write made no progress\n", stderr);
            result = EXIT_FAILURE;
            break;
        }
        sent += (size_t)count;
    }
    close(fd[1]); /* The child receives EOF after reading all remaining data. */
    int status;
    if (wait_child(pid, &status) == -1)
        return EXIT_FAILURE;
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
        result = EXIT_FAILURE;
    return result;
}
