#define _POSIX_C_SOURCE 200809L
#include <arpa/inet.h>
#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

/* Build with: gcc -std=c11 -Wall -Wextra -Wpedantic FILE.c -o PROGRAM
   Run server: ./server 192.168.1.10 5050
   Run client: ./client 192.168.1.10 5050
   Replace the example IP with the server computer's actual LAN IPv4 address.
   Both users can type at any time. Ctrl+D ends sending; Ctrl+C exits.
   Plain TCP: messages are not encrypted. */

static int parse_address(const char *ip, const char *port,
                         struct sockaddr_in *address)
{
    char *end;
    errno = 0;
    long number = strtol(port, &end, 10);
    if (errno || end == port || *end != '\0' || number < 1 || number > 65535) {
        fputs("Port must be an integer from 1 to 65535\n", stderr);
        return -1;
    }
    address->sin_family = AF_INET;
    address->sin_port = htons((unsigned short)number);
    if (inet_pton(AF_INET, ip, &address->sin_addr) != 1) {
        fputs("Use a numeric IPv4 address, for example 192.168.1.10\n", stderr);
        return -1;
    }
    return 0;
}

static int send_all(int fd, const char *data, size_t length)
{
    while (length > 0) {
        ssize_t count = send(fd, data, length, MSG_NOSIGNAL);
        if (count == -1 && errno == EINTR)
            continue;
        if (count <= 0) {
            if (count == -1)
                perror("send");
            else
                fputs("send made no progress\n", stderr);
            return -1;
        }
        data += count;
        length -= (size_t)count;
    }
    return 0;
}

/* poll lets either user send first, without threads or alternating turns.
   TCP is a byte stream: receive chunks need not match typed lines. */
static int chat(int fd)
{
    struct pollfd events[2] = {
        { .fd = STDIN_FILENO, .events = POLLIN },
        { .fd = fd, .events = POLLIN }
    };
    int sending = 1;
    int receiving = 1;
    while (sending || receiving) {
        if (poll(events, 2, -1) == -1) {
            if (errno == EINTR)
                continue;
            perror("poll");
            return EXIT_FAILURE;
        }
        if (events[1].revents & (POLLIN | POLLHUP | POLLERR)) {
            char buffer[1024];
            ssize_t count = recv(fd, buffer, sizeof(buffer), 0);
            if (count > 0) {
                if (fwrite(buffer, 1, (size_t)count, stdout) != (size_t)count ||
                    fflush(stdout) == EOF) {
                    perror("stdout");
                    return EXIT_FAILURE;
                }
            } else if (count == 0) {
                fputs("Peer finished sending. You can still send; Ctrl+D to exit.\n", stderr);
                receiving = 0;
                events[1].fd = -1;
            } else if (errno != EINTR) {
                perror("recv");
                return EXIT_FAILURE;
            }
        }
        if (events[0].revents & (POLLIN | POLLHUP | POLLERR)) {
            char buffer[1024];
            ssize_t count = read(STDIN_FILENO, buffer, sizeof(buffer));
            if (count > 0) {
                if (send_all(fd, buffer, (size_t)count) == -1)
                    return EXIT_FAILURE;
            } else if (count == 0) {
                /* Half-close: still receive messages after local input ends. */
                if (shutdown(fd, SHUT_WR) == -1) {
                    perror("shutdown");
                    return EXIT_FAILURE;
                }
                sending = 0;
                events[0].fd = -1;
                fputs("Finished sending; waiting for peer to finish.\n", stderr);
            } else if (errno != EINTR) {
                perror("read stdin");
                return EXIT_FAILURE;
            }
        }
        if ((events[0].revents | events[1].revents) & POLLNVAL) {
            fputs("Invalid chat file descriptor\n", stderr);
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s SERVER_LAN_IP PORT\n", argv[0]);
        return EXIT_FAILURE;
    }
    struct sockaddr_in address = {0};
    if (parse_address(argv[1], argv[2], &address) == -1)
        return EXIT_FAILURE;
    int listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener == -1) {
        perror("socket");
        return EXIT_FAILURE;
    }
    int result = EXIT_FAILURE;
    int reuse = 1;
    if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) {
        perror("setsockopt");
        goto done;
    }
    /* This IP must belong to the server computer, not the client computer. */
    if (bind(listener, (struct sockaddr *)&address, sizeof(address)) == -1) {
        perror("bind");
        goto done;
    }
    if (listen(listener, 1) == -1) {
        perror("listen");
        goto done;
    }
    fprintf(stderr, "Listening on %s:%s; waiting for one chat client...\n", argv[1], argv[2]);
    int peer;
    do {
        peer = accept(listener, NULL, NULL);
    } while (peer == -1 && errno == EINTR);
    if (peer == -1) {
        perror("accept");
        goto done;
    }
    close(listener);
    listener = -1;
    fputs("Connected. Type a message and press Enter. Ctrl+D finishes sending.\n", stderr);
    result = chat(peer);
    close(peer);
done:
    if (listener != -1)
        close(listener);
    return result;
}
