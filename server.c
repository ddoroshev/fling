#include <arpa/inet.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include "server.h"

static int bind_listener(int, int);
static void set_listener_options(int);
static void set_client_sock_options(int);


/**
 * Create and set up a listening TCP socket
 *
 * Creates a TCP socket, sets socket options, binds it to the specified port
 * on all network interfaces (0.0.0.0), and puts it into listening mode.
 * Configures the socket with reuse address option for fast restarts.
 *
 * @param port Port number to bind the socket to
 * @return Socket file descriptor on success, -1 on error
 */
int start_listener(int port)
{
    int listener, rc;
    int backlog = 10;

    listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener < 0) {
        perror("socket");
        return -1;
    }

    set_listener_options(listener);

    rc = bind_listener(listener, port);
    if (rc < 0) {
        close(listener);
        return -1;
    }

    rc = listen(listener, backlog);
    if (rc < 0) {
        perror("listen");
        return -1;
    }
    printf("Listening on 0:%d...\n", port);
    return listener;
}

/**
 * Bind the listener socket to 0.0.0.0:port
 *
 * Sets up the `sockaddr_in` structure to bind the socket to all network
 * interfaces (`INADDR_ANY`) on the specified port number.
 *
 * @param listener Socket file descriptor to bind
 * @param port     Port number to bind to
 *
 * @return 0 on success, -1 on error
 */
static int bind_listener(int listener, int port)
{
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listener, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return -1;
    }
    return 0;
}

/**
 * Accept a client connection on the listener socket
 *
 * Accepts an incoming connection, logs the client's IP address and port,
 * and configures the new socket with performance-oriented options.
 * The socket is set up with `TCP_NODELAY` and larger receive buffers.
 *
 * @param listener Listening socket file descriptor
 *
 * @return New socket file descriptor for the client on success, -1 on error
 */
int accept_connection(int listener)
{
    int sock;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    sock = accept(listener, (struct sockaddr*)&client_addr, &client_addr_len);
    if (sock < 0) {
        perror("accept");
        return -1;
    }

    /* Get client IP */
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
    printf(
        "Accepted connection from client of %s:%d\n",
        client_ip, ntohs(client_addr.sin_port)
    );
    set_client_sock_options(sock);

    return sock;
}

/**
 * Set socket options for the listener socket
 *
 * Configures the listener socket with the `SO_REUSEADDR` option to allow
 * quick restart of the server by reusing the address even if it's
 * in `TIME_WAIT`.
 *
 * @param listener Listening socket file descriptor
 */
static void set_listener_options(int listener)
{
    int opt = 1, rc;
    rc = setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (rc < 0) {
        perror("setsockopt SO_REUSEADDR");
    }
}

/**
 * Configure socket options for client connections
 *
 * Sets performance-oriented socket options on client connections:
 * - Increases receive buffer size to 512KB for better throughput
 * - Enables `TCP_NODELAY` to disable Nagle's algorithm for lower latency
 *
 * No error checking on the return value as these are optimizations
 * and the transfer can proceed even if they fail.
 *
 * @param sock Client socket file descriptor
 */
static void set_client_sock_options(int sock)
{
    int flag = 1, rc;
    int recv_buf_size = 1024 * 512;

    rc = setsockopt(sock, SOL_SOCKET, SO_RCVBUF,
                    &recv_buf_size, sizeof(recv_buf_size));
    if (rc < 0) {
        perror("setsockopt SO_RCVBUF");
    }

    rc = setsockopt(sock, IPPROTO_TCP, TCP_NODELAY,
                    &flag, sizeof(flag));
    if (rc < 0) {
        perror("setsockopt TCP_NODELAY");
    }
}
