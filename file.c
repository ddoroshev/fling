#include <inttypes.h>
#include <libgen.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>

#include "file.h"
#include "fsock.h"
#include "progress.h"

int file_open(file *f, char *fname)
{
    int fd;
    struct stat file_stat;

    fd = open(fname, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return -1;
    }
    if (fstat(fd, &file_stat) == -1) {
        perror("fstat");
        close(fd);
        return -1;
    }
    if (!S_ISREG(file_stat.st_mode)) {
        printf("%s is not a regular file\n", fname);
        close(fd);
        return -1;
    }
    strncpy(f->hdr.fname, basename(fname), sizeof(f->hdr.fname) - 1);
    f->hdr.fsize = (size_t)file_stat.st_size;
    f->fd = fd;
    return 0;
}

/**
 * Send file contents over socket with progress tracking
 *
 * Reads the file in chunks and sends each chunk over the socket until
 * the entire file is transferred. Updates progress bar if callback is set.
 * Uses `ftosock()` to handle the actual data transfer for each chunk.
 *
 * @param f    Pointer to file structure with open file descriptor
 * @param sock Socket descriptor to send data to
 * @return     Total bytes sent on success, -1 on error
 */
static ssize_t file_send_contents(file *f, int sock)
{
    size_t offset = 0;
    char buf[CHUNK_SIZE] = {0};

    while (offset < f->hdr.fsize) {
        ssize_t bytes_sent = ftosock(f->fd, sock, buf, CHUNK_SIZE);
        if (bytes_sent < 0) {
            return -1;
        }
        offset += (size_t)bytes_sent;
        if (progress_bar_callback) {
            progress_bar_callback(offset, f->hdr.fsize);
        }
    }

    return (ssize_t)offset;
}

ssize_t file_send(file *f, int sock)
{
    if (send(sock, &f->hdr, FHEADER_SIZE, 0) < 0) {
        perror("send header");
        return -1;
    }

    return file_send_contents(f, sock);
}

/**
 * Create a new file or truncate existing one for writing
 *
 * Creates a new file using the filename stored in the file header structure.
 * If the file already exists, it will be truncated to zero length.
 * The file is created with standard permissions (0644).
 *
 * @param f Pointer to file structure with filename in header
 * @returns 0 on success, -1 on error
 */
static int file_create(file *f)
{
    int fd = open(f->hdr.fname, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open");
        return -1;
    }
    f->fd = fd;
    return 0;
}

void file_close(file *f)
{
    close(f->fd);
    f->fd = 0;
}

/**
 * Receive file data from socket and write to file
 *
 * Reads data from the socket in chunks and writes it to the file
 * until the expected number of bytes (f->hdr.fsize) is received.
 * Uses socktof() to handle the actual data transfer for each chunk.
 *
 * @param f Pointer to file structure with open file descriptor and size info
 * @param sock Socket descriptor to receive data from
 * @return Total bytes received on success, -1 on error
 */
static ssize_t file_receive_contents(file *f, int sock)
{
    size_t left = f->hdr.fsize;
    char buf[CHUNK_SIZE] = {0};

    while (left > 0) {
        size_t chunk_size = CHUNK_SIZE <= left ? CHUNK_SIZE : left;
        ssize_t bytes_read = socktof(sock, f->fd, buf, chunk_size);
        if (bytes_read < 0) {
            return -1;
        }
        left -= (size_t)bytes_read;
    }
    return (ssize_t)f->hdr.fsize;
}

ssize_t receive_file(int sock)
{
    ssize_t bytes_read, rc, retval;
    file f = {0};
    char clean_name[MAX_FILE_NAME + 1];
    const char *base;

    bytes_read = recv(sock, &f, FHEADER_SIZE, 0);
    if (bytes_read <= 0 || (size_t)bytes_read < FHEADER_SIZE) {
        printf("Unexpected amount of bytes: %zd\n", bytes_read);
        return -1;
    }
    f.hdr.fname[MAX_FILE_NAME] = '\0';

    base = basename(f.hdr.fname);
    strncpy(clean_name, base, MAX_FILE_NAME);
    clean_name[MAX_FILE_NAME] = '\0';

    strncpy(f.hdr.fname, clean_name, MAX_FILE_NAME);

    printf("Accepting file: name %s, size %zd...\n",
           f.hdr.fname, f.hdr.fsize);

    retval = (ssize_t)f.hdr.fsize;

    rc = file_create(&f);
    if (rc < 0) {
        retval = rc;
        goto ret;
    }
    rc = file_receive_contents(&f, sock);
    if (rc < 0) {
        retval = rc;
        goto fclose;
    }
    printf("File %s received successfully\n", f.hdr.fname);

fclose:
    file_close(&f);
ret:
    return retval;
}
