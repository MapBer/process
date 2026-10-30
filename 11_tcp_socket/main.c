#define _POSIX_C_SOURCE 200809L
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 5050

/* TCP can send fewer bytes than requested, even for a small message. */
static int send_all(int fd, const char *data, size_t length)
{
    while (length > 0) {
        ssize_t sent = send(fd, data, length, MSG_NOSIGNAL);
        if (sent == -1) {
            if (errno == EINTR)
                continue;
            perror("send");
            return -1;
        }
        if (sent == 0) {
            fputs("send made no progress\n", stderr);
            return -1;
        }
        data += sent;
        length -= (size_t)sent;
    }
    return 0;
}

static int receive_all(int fd)
{
    char buffer[128];
    for (;;) {
        ssize_t count = recv(fd, buffer, sizeof(buffer), 0);
        if (count == 0)
            return fflush(stdout) == EOF ? -1 : 0;
        if (count == -1) {
            if (errno == EINTR)
                continue;
            perror("recv");
            return -1;
        }
        /* recv() returns bytes, without adding a null terminator. */
        if (fwrite(buffer, 1, (size_t)count, stdout) != (size_t)count) {
            perror("fwrite");
            return -1;
        }
    }
}

int main(int argc, char **argv)
{
    if (argc != 2 || (strcmp(argv[1], "server") != 0 &&
                      strcmp(argv[1], "client") != 0)) {
        fprintf(stderr, "Usage: %s server|client\n", argv[0]);
        return EXIT_FAILURE;
    }
    int server = strcmp(argv[1], "server") == 0;
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket");
        return EXIT_FAILURE;
    }
    struct sockaddr_in address = {0};
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT); /* TCP ports use network byte order. */
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    int result = EXIT_FAILURE;

    if (server) {
        int reuse = 1;
        if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) {
            perror("setsockopt");
            goto done;
        }
        if (bind(fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
            perror("bind");
            goto done;
        }
        if (listen(fd, 1) == -1) {
            perror("listen");
            goto done;
        }
        printf("Listening on 127.0.0.1:%d\n", PORT);
        fflush(stdout);
        int peer;
        do {
            peer = accept(fd, NULL, NULL);
        } while (peer == -1 && errno == EINTR);
        if (peer == -1) {
            perror("accept");
            goto done;
        }
        /* Client half-closes its write direction to mark the end of request. */
        if (receive_all(peer) == 0) {
            const char reply[] = "Hello from TCP server\n";
            if (send_all(peer, reply, sizeof(reply) - 1) == 0)
                result = EXIT_SUCCESS;
        }
        close(peer); /* Closing the connection gives the client EOF. */
    } else {
        if (connect(fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
            perror("connect");
            goto done;
        }
        const char message[] = "Hello from TCP client\n";
        if (send_all(fd, message, sizeof(message) - 1) == -1)
            goto done;
        /* Keep the read direction open so the reply can still arrive. */
        if (shutdown(fd, SHUT_WR) == -1) {
            perror("shutdown");
            goto done;
        }
        if (receive_all(fd) == 0)
            result = EXIT_SUCCESS;
    }
done:
    close(fd);
    return result;
}
