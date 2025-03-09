#include "test_e2e.h"

void run_e2e(void)
{
    FLING_TEST_SEND("file-0.dat");
    FLING_TEST_SEND("file-1k.dat");
    FLING_TEST_SEND("file-1M.dat");
    FLING_TEST_SEND("file-10M.dat");

    if (run_slow_tests) {
        FLING_TEST_SEND("file-100M.dat");
        FLING_TEST_SEND("file-1G.dat");
        FLING_TEST_SEND("file-5G.dat");
        FLING_TEST_SEND("file-10G.dat");
    }
}
