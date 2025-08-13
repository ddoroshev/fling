#pragma once

#include <stdint.h>

#define MAX_FILE_NAME 255

typedef struct {
    char   fname[MAX_FILE_NAME + 1];
    size_t fsize;
} file_header;

typedef struct {
    file_header hdr;
    int fd;
} file;

#define FHEADER_SIZE (size_t)sizeof(file_header)
#define CHUNK_SIZE   1024*256

/**
 * Receive file with header from socket and save it
 *
 * @param sock Socket descriptor to receive data from
 * @return Size of received file in bytes on success, -1 on error
 *
 * Receives the file header first, then creates a new file with the
 * received filename and writes the file contents to it. The function
 * handles the entire file receiving process, from header to content.
 */
ssize_t receive_file(int sock);

/**
 * Open a file and prepare its header for transfer
 *
 * Opens the specified file, reads its metadata (size), and populates
 * the file structure with filename and size information. The filename
 * is extracted from the path and copied to the header structure.
 *
 * @param f     Pointer to file structure to be filled
 * @param fname Path to the file to be opened
 * @return 0 on success, -1 on error (e.g., file not found)
 */
int file_open  (file*, char*);

/**
 * Close a file descriptor and reset the file structure
 *
 * Closes the file descriptor stored in the file structure and
 * resets the descriptor to 0 to prevent accidental reuse.
 *
 * @param f Pointer to file structure with open file descriptor
 */
void file_close (file*);

/**
 * Send file header and contents over socket
 *
 * @param f Pointer to file structure with open file and prepared header
 * @param sock Socket descriptor to send data to
 *
 * First sends the file header containing filename and size information,
 * then sends the file contents by calling file_send_contents().
 *
 * Return: Total bytes sent on success, -1 on error
 */
ssize_t file_send(file*, int sock);
