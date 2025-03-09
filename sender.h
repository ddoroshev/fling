#pragma once

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
int exec_sender(char *filename, const char *host, const char *port);
