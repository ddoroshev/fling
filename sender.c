#include <stdio.h>
#include <unistd.h>

#include "client.h"
#include "debug.h"
#include "file.h"
#include "progress.h"

/**
 * Execute the file sending process
 *
 * Opens the file, establishes a connection with the receiver,
 * sends the file with progress tracking, and cleans up resources.
 * The entire sending process is handled, from file opening to socket closing.
 *
 * @param filename Path to the file to send
 * @param host     Hostname or IP address of the receiver
 * @param port     Port number as a string
 *
 * @return 0 on success, 1 on error
 */
int exec_sender(char *filename, const char *host, const char *port)
{
    int retval = 0, rc, sock;
    file f = {0};

    ssize_t total_size;

    rc = file_open(&f, filename);
    if (rc < 0) {
        return 1;
    }

    sock = establish_connection(host, port);
    if (sock < 0) {
        file_close(&f);
        return 1;
    }

    start_progress_bar();

    total_size = file_send(&f, sock);
    if (total_size >= 0) {
        stop_progress_bar((size_t)total_size);
    } else {
        retval = total_size;
    }

    /* Cleanup */
    close(sock);
    file_close(&f);

    return retval;
}
