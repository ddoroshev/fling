#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "server.h"
#include "file.h"
#include "receiver.h"

/**
 * Execute the file receiving server process
 * 
 * Sets up a listening socket, then enters an infinite loop to
 * accept connections and receive files. Each client connection
 * is handled sequentially. After receiving a file, the connection
 * is closed and the server waits for the next connection.
 *
 * @param port Port number to listen on
 *
 * @return 0 on normal exit, -1 on startup error
 */
int exec_receiver(int port)
{
    int listener;

    listener = start_listener(port);
    if (listener < 0) {
        return -1;
    }

    /* Start accepting connections */
    while (1) {
        int sock = accept_connection(listener);
        if (sock < 0) {
            continue;
        }

        receive_file(sock);
        close(sock);
    }

    close(listener);

    printf("\nShutting down...\n");
    return 0;
}

