#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void)
{
    printf("PID: %ld, PPID: %ld\n", (long)getpid(), (long)getppid());
    puts("Waiting for 20 seconds. Run ps in another terminal.");
    fflush(stdout);
    unsigned int remaining = 20;
    while ((remaining = sleep(remaining)) != 0) {
        /* Resume sleeping if interrupted by a signal. */
    }
    return EXIT_SUCCESS;
}
