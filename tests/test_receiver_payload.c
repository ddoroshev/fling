#include <assert.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "../client.h"
#include "../file.h"

#include "test.h"
#include "test_receiver_payload.h"

/**
 * Send a malformed payload with attempt of path traversal
 */
static void test_path_traversal_payload(void)
{
    SETUP();

    file_header hdr = {
        .fname = "../" TEST_FNAME_PATH_TRAVERSAL,
        .fsize = 0,
    };
    int is_file_outside = 0;
    int is_file_inside = 0;

    SEND(ctx.sock, &hdr, FHEADER_SIZE);
    WAITABIT();

    is_file_outside = access(TEST_FNAME_PATH_TRAVERSAL, F_OK) == 0;
    is_file_inside = access("tests/data/" TEST_FNAME_PATH_TRAVERSAL, F_OK) == 0;

    CHECK(!is_file_outside, "File was found outside the test dir");
    CHECK(is_file_inside, "File was found inside the test dir");

    TEARDOWN();
}

/**
 * Send payloads with file size mismatch, when the actual file size
 * is larger than specified in the header.
 */
static void test_file_size_mismatch__actual_size_is_larger_1(void)
{
    SETUP();

    size_t header_size = 2,
           actual_size = 5;
    int fd, len;
    file_header hdr = {
        .fname = TEST_FNAME_SIZE_MISMATCH_1_1,
        .fsize = header_size,
    };
    memset(ctx.buf, 'a', actual_size);

    SEND(ctx.sock, &hdr, FHEADER_SIZE);
    SEND(ctx.sock, ctx.buf, actual_size);

    WAITABIT();
    memset(ctx.buf, 0, actual_size);
    OPEN(fd, "tests/data/" TEST_FNAME_SIZE_MISMATCH_1_1, O_RDONLY);
    len = read(fd, ctx.buf, actual_size);
    CHECK(len == header_size, "File size is incorrect (%d)", len);
    close(fd);

    TEARDOWN();
}

/**
 * Send payloads with file size mismatch, when the actual file size
 * is larger than specified in the header, causing additional iteration
 * of the receiver to get the second chunk.
 */
static void test_file_size_mismatch__actual_size_is_larger_2(void)
{
    SETUP();

    size_t header_size = CHUNK_SIZE + 2,
           actual_size = CHUNK_SIZE + 5;
    int fd, len;
    file_header hdr = {
        .fname = TEST_FNAME_SIZE_MISMATCH_1_2,
        .fsize = header_size,
    };

    memset(ctx.buf, 'a', actual_size);

    /* Action */
    SEND(ctx.sock, &hdr, FHEADER_SIZE);
    SEND(ctx.sock, ctx.buf, actual_size);

    /* Check */
    WAITABIT();
    memset(ctx.buf, 0, actual_size);
    OPEN(fd, "tests/data/" TEST_FNAME_SIZE_MISMATCH_1_2, O_RDONLY);
    len = read(fd, ctx.buf, actual_size);
    CHECK(len == header_size, "File size is incorrect (%d)", len);
    close(fd);

    TEARDOWN();
}

/**
 * Send payloads with file size mismatch, when the actual file size
 * is smaller than specified in the header.
 */
static void test_file_size_mismatch__actual_size_is_smaller(void)
{
    SETUP();

    size_t header_size = 3,
           actual_size = 5;
    int fd, len;
    file_header hdr = {
        .fname = TEST_FNAME_SIZE_MISMATCH_2,
        .fsize = actual_size,
    };

    memset(ctx.buf, 'a', actual_size);

    /* Action */
    SEND(ctx.sock, &hdr, FHEADER_SIZE);
    SEND(ctx.sock, ctx.buf, header_size);

    /* Check */
    WAITABIT();
    memset(ctx.buf, 0, actual_size);
    OPEN(fd, "tests/data/" TEST_FNAME_SIZE_MISMATCH_2, O_RDONLY);
    len = read(fd, ctx.buf, actual_size);
    CHECK(len == header_size, "File size is incorrect (%d)", len);
    close(fd);

    TEARDOWN();
}

/**
 * Run tests composing different kinds of payload,
 * including incorrect and malicious ones
 */
void run_receiver_payload_tests(void)
{
    test_path_traversal_payload();
    test_file_size_mismatch__actual_size_is_larger_1();
    test_file_size_mismatch__actual_size_is_larger_2();
    test_file_size_mismatch__actual_size_is_smaller();
}
