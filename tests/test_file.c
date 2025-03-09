#include <string.h>
#include <unistd.h>
#include <inttypes.h>

#include "../file.h"

#include "test.h"
#include "test_file.h"

static void test_file_open__success(void)
{
    file f = {0};
    int rc;

    rc = file_open(&f, "tests/gen-data/" FILE_NAME_1M);

    CHECK(rc >= 0, "Unexpected result code: %d", rc);
    rc = strcmp(f.hdr.fname, FILE_NAME_1M);
    CHECK(rc == 0, "Unexpected result code: %d", rc);
    CHECK(f.hdr.fsize == 1024 * 1024, "Unexpected file size: %" PRIu64, f.hdr.fsize);

    close(f.fd);
}

static void test_file_open__not_found(void)
{
    file f = {0};
    int rc;

    rc = file_open(&f, "tests/gen-data/foobar");

    CHECK(rc == -1, "Unexpected result code: %d", rc);
}

static void test_file_open__unsupported_dir(void)
{
    file f = {0};
    int rc;

    rc = file_open(&f, "tests/gen-data");

    CHECK(rc == -1, "Unexpected result code: %d", rc);
}

static void test_file_open__unsupported_dev(void)
{
    file f = {0};
    int rc;

    rc = file_open(&f, "/dev/null");

    CHECK(rc == -1, "Unexpected result code: %d", rc);
}

static void test_file_open__255(void)
{
    file f = {0};
    int rc;

    rc = file_open(&f, "tests/gen-data/" FILE_NAME_255);

    CHECK(rc >= 0, "Unexpected result code: %d", rc);
    rc = strcmp(f.hdr.fname, FILE_NAME_255);
    CHECK(rc == 0, "Unexpected result code: %d", rc);
    CHECK(f.hdr.fsize == 0, "Unexpected file size: %" PRIu64, f.hdr.fsize);

    close(f.fd);
}

static void test_file_close__success(void)
{
    file f = {0};
    file_open(&f, "tests/gen-data/" FILE_NAME_1M);

    file_close(&f);

    CHECK(f.fd == 0, "Unexpected fd: %d", f.fd);
}

static void test_file_close__not_opened(void)
{
    file f = {0};

    file_close(&f);

    CHECK(f.fd == 0, "Unexpected fd: %d", f.fd);
}

void run_file_tests(void)
{
    test_file_open__success();
    test_file_open__not_found();
    test_file_open__unsupported_dir();
    test_file_open__unsupported_dev();
    test_file_open__255();
    test_file_close__success();
    test_file_close__not_opened();
}
