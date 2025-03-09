#pragma once

#include <stdlib.h>

#include "test.h"

void run_e2e(void);

#define FLING_TEST_SEND(fname) \
    system("bin/fling send tests/gen-data/" fname " 127.0.0.1 54321"); \
    system("diff tests/data/" fname " tests/gen-data/" fname " " \
           "&& printf '" OK " - "fname"\n' " \
           "|| printf '" FAIL " - "fname"\n'");
