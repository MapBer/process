# Communicate through a TCP socket

Run the same executable in two independent processes: a server and a client.
The server handles one connection and exits. It binds to `127.0.0.1:5050`, so
this example communicates only on the local machine.

1. Both processes create an IPv4 TCP socket using `socket(AF_INET, SOCK_STREAM, 0)`.
2. The server uses `bind()`, `listen()`, and `accept()`. The accepted socket carries data; the listening socket accepts connections.
3. The client uses `connect()` and sends a greeting using `send()`.
4. The client calls `shutdown(fd, SHUT_WR)`, ending its request while keeping its read direction open.
5. The server reads using `recv()` until EOF, sends a reply, and closes the connection.
6. The client reads the reply until EOF and closes its socket.

## Build and run

From the project root:

```bash
make -C exercises 11_tcp_socket
```

Start the server in terminal 1:

```bash
./exercises/bin/11_tcp_socket server
```

Then start the client in terminal 2:

```bash
./exercises/bin/11_tcp_socket client
```

Server output:

```text
Listening on 127.0.0.1:5050
Hello from TCP client
```

Client output:

```text
Hello from TCP server
```

TCP is an ordered byte stream: one `send()` does not correspond to one `recv()`.
The code handles partial sends, interrupted operations, and receives in a loop.
This example uses EOF to delimit each message; persistent connections would
need framing, such as a length prefix. `MSG_NOSIGNAL` prevents Linux from
terminating the sender with `SIGPIPE` if the peer closes; `send()` reports an
error instead.

If `connect()` reports connection refused, start the server first. If `bind()`
reports address already in use, stop the other server or change `PORT` in the
source and rebuild. `SO_REUSEADDR` helps with restarts after a connection closes;
it does not allow a second active listener on the same address.

Question: Why would both processes wait forever if the client omitted
`shutdown()` and waited for a reply while the server waited for EOF?

Extension: Accept multiple clients in a loop, closing each accepted socket.

Read local documentation with `man 2 socket`, `man 2 bind`, `man 2 accept`,
`man 2 connect`, `man 2 send`, `man 2 recv`, and `man 2 shutdown`.
