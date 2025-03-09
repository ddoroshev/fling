#pragma once

#define SOCKET_BUF_SIZE 1024*512

/**
 * Create a TCP socket and connect to specified host
 *
 * This function creates a TCP socket, sets socket options for performance,
 * resolves the host name to an IP address, and establishes a connection.
 * The socket is optimized with TCP_NODELAY and larger buffer sizes.
 *
 * @param host Host name or IP address to connect to
 * @param port Port number as a string
 *
 * @return Socket file descriptor on success, -1 on error
 */
int establish_connection(const char *host, const char *port);
