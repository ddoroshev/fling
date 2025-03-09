#pragma once

#include <sys/types.h>

/**
 * Read from file and send data to socket
 *
 * Reads up to `length` bytes from the file descriptor and sends
 * the data to the socket. This is the core transfer mechanism for
 * sending files over the network.
 *
 * @param fd     File descriptor to read from
 * @param sock   Socket descriptor to send data to
 * @param buf    Buffer to use for the data transfer
 * @param length Maximum number of bytes to read/send (buffer size)
 *
 * @return Number of bytes sent on success, -1 on error
 */
ssize_t ftosock(int fd, int sock, char *buf, size_t length);

/**
 * Receive data from socket and write to file
 *
 * Receives up to `length` bytes from the socket and writes the data
 * to the file descriptor. This is the core transfer mechanism for
 * receiving files over the network.
 *
 * @param sock   Socket descriptor to receive data from
 * @param fd     File descriptor to write to
 * @param buf    Buffer to use for the data transfer
 * @param length Maximum number of bytes to receive/write (buffer size)
 *
 * @return Number of bytes written to file on success, -1 on error
 */
ssize_t socktof(int sock, int fd, char *buf, size_t length);
