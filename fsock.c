#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>

#include "fsock.h"

ssize_t ftosock(int fd, int sock, char *buf, size_t length)
{
    ssize_t bytes_read, bytes_sent;

    bytes_read = read(fd, buf, length);
    if (bytes_read <= 0) {
        if (bytes_read < 0) {
            perror("read");
        }
        return -1;
    }

    bytes_sent = send(sock, buf, bytes_read, 0);
    if (bytes_sent < 0) {
        perror("send");
        return -1;
    }
    return bytes_sent;
}

ssize_t socktof(int sock, int fd, char *buf, size_t length)
{
    ssize_t bytes_read, bytes_written;

    bytes_read = recv(sock, buf, length, 0);
    if (bytes_read <= 0) {
        perror("recv");
        return -1;
    }

    bytes_written = write(fd, buf, bytes_read);
    if (bytes_written < 1) {
        perror("write");
        return -1;
    }

    return bytes_written;
}
