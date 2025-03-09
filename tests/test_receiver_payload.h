#pragma once

#include "../file.h"

#define TEST_FNAME_PATH_TRAVERSAL    "fname-traversal.dat"
#define TEST_FNAME_SIZE_MISMATCH_1_1 "file-size-mismatch-1-1.dat"
#define TEST_FNAME_SIZE_MISMATCH_1_2 "file-size-mismatch-1-2.dat"
#define TEST_FNAME_SIZE_MISMATCH_2   "file-size-mismatch-2.dat"

#define SEND(sock, buf, size) \
    do { \
        if (send(sock, buf, size, 0) < 0) { \
            perror("send"); \
            goto _teardown; \
        } \
    } while (0)

#define OPEN(fd, fname, mode) \
    do { \
        fd = open(fname, mode); \
        if (fd < 0) { \
            perror("open"); \
            goto _teardown; \
        } \
    } while (0)

static struct {
    int sock;
    char buf[CHUNK_SIZE * 2];
} ctx;

#define SETUP() do { \
    ctx.sock = establish_connection("127.0.0.1", "54321"); \
    assert(ctx.sock >= 0); \
} while (0)

#define TEARDOWN() \
_teardown: \
    do { close(ctx.sock); } while (0)

void run_receiver_payload_tests(void);
