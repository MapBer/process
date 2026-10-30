#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void)
{
    /* IPC_PRIVATE creates a new segment; 0600 restricts access to its owner. */
    int shmid = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0600);
    if (shmid == -1) {
        perror("shmget");
        return EXIT_FAILURE;
    }

    int *count = shmat(shmid, NULL, 0);
    if (count == (void *)-1) {
        perror("shmat");
        if (shmctl(shmid, IPC_RMID, NULL) == -1)
            perror("shmctl");
        return EXIT_FAILURE;
    }

    /* Delete automatically after the last detach, including on process exit.
       The existing attachment remains valid and is inherited across fork(). */
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl");
        if (shmdt(count) == -1)
            perror("shmdt");
        return EXIT_FAILURE;
    }

    *count = 10;
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        if (shmdt(count) == -1)
            perror("shmdt");
        return EXIT_FAILURE;
    }
    if (pid == 0) {
        *count += 5; /* This writes to the same shared segment the parent sees. */
        if (shmdt(count) == -1) {
            perror("shmdt");
            _exit(EXIT_FAILURE);
        }
        _exit(EXIT_SUCCESS);
    }

    int status;
    int result = EXIT_SUCCESS;
    /* Wait before accessing count, so parent and child do not race. */
    while (waitpid(pid, &status, 0) == -1) {
        if (errno == EINTR)
            continue;
        perror("waitpid");
        result = EXIT_FAILURE;
        break;
    }
    if (result == EXIT_SUCCESS) {
        if (!WIFEXITED(status) || WEXITSTATUS(status) != EXIT_SUCCESS) {
            fputs("Child did not complete successfully\n", stderr);
            result = EXIT_FAILURE;
        } else if (printf("Parent sees shared count = %d\n", *count) < 0 ||
                   fflush(stdout) == EOF) {
            perror("stdout");
            result = EXIT_FAILURE;
        }
    }
    if (shmdt(count) == -1) {
        perror("shmdt");
        result = EXIT_FAILURE;
    }
    return result;
}
