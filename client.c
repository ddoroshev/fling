#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include "debug.h"
#include "client.h"

static void set_sock_options(int sock);

int establish_connection(const char *host, const char *port)
{
    int sock, err, rc;
    struct addrinfo hints = {0}, *res;

    /* Prepare the socket */
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return -1;
    }

    set_sock_options(sock);

    /* Get address info from arguments */
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    DPRINT("addrinfo...");
    err = getaddrinfo(host, port, &hints, &res);
    if (err) {
        close(sock);
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(err));
        return -1;
    }
    DPRINT("connect...");
    /* Connect to the reciever */
    /* TODO: Control the connect timeout */
    rc = connect(sock, res->ai_addr, res->ai_addrlen);
    freeaddrinfo(res);

    if (rc < 0) {
        perror("connect");
        close(sock);
        return -1;
    }

    return sock;
}

/**
 * Configure socket options for optimal performance
 *
 * @param sock Socket file descriptor to configure
 *
 * Sets several socket options to improve performance:
 * - Increases send buffer size for better throughput
 * - Enables address reuse to avoid "address already in use" errors
 * - Enables TCP_NODELAY to disable Nagle's algorithm for lower latency
 *
 * No error checking on the return value as these are optimizations
 * and the transfer can proceed even if they fail.
 */
static void set_sock_options(int sock)
{
    int buf_size = SOCKET_BUF_SIZE, yes = 1, rc;

    rc = setsockopt(sock, SOL_SOCKET, SO_SNDBUF,
                    &buf_size, sizeof(buf_size));
    if (rc < 0) {
        perror("setsockopt SO_SNDBUF");
    }
    rc = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
                    &yes, sizeof(yes));
    if (rc < 0) {
        perror("setsockopt SO_REUSEADDR");
    }

    rc = setsockopt(sock, IPPROTO_TCP, TCP_NODELAY,
                    &yes, sizeof(yes));
    if (rc < 0) {
        perror("setsockopt TCP_NODELAY");
    }
}
