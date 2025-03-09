#pragma once

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

#define OK    "\033[0;32mOK\033[0m"
#define FAIL  "\033[0;31mFAIL\033[0m"
#define ERROR "\033[0;31mERROR\033[0m"

extern int test_failures;

#define CHECK(chk, fmt, ...) \
    if (chk) { \
        printf(OK " [%s:%s] - " #chk "\n", __FILE__, __func__); \
    } else { \
        printf(FAIL " [%s:%s] - " #chk " - " fmt "\n", __FILE__, __func__, ##__VA_ARGS__); \
        test_failures++; \
    }

#define WAITABIT() usleep(100000)

extern int run_slow_tests;
extern pid_t pid_test_server;
