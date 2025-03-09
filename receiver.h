#pragma once

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
int exec_receiver(int port);
