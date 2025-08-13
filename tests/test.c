/**************************************************************
 * A test program that starts new filer instance as a server, *
 * submits the prepared file and checks that it has been      *
 * submitted successfully.                                    *
 **************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "test.h"
#include "test_e2e.h"
#include "test_receiver_payload.h"
#include "test_file.h"

int run_slow_tests = 0;
pid_t pid_test_server;
int test_failures = 0;

static void run_tests(void)
{
    run_e2e();
    run_receiver_payload_tests();
    run_file_tests();
}

static void _cleanup(void)
{
    /*
     * Kill the server process
     */
    if (pid_test_server > 0) {
        WAITABIT();
        puts("Killing the servier fling process...");
        kill(pid_test_server, SIGTERM);
    }
}

/**
 * Perform a cleanup after failing assert()
 */
static void handle_sigabrt()
{
    _cleanup();
    exit(1);
}

/*
 * main - Test program for the file transfer system
 *
 * Starts a filer (receiver) process in a child process, then sends multiple
 * test files of different sizes to it using the sender program. After transfer,
 * verifies all files using diff to ensure content integrity. Finally,
 * terminates the filer process.
 *
 * Files are tested in increasing size order, from 1KB to 1GB.
 * Larger files (5GB, 10GB) are commented out by default due to time constraints.
 *
 * Return: 0 on success, 1 on error
 */
int main(int argc, const char *argv[])
{
    signal(SIGABRT, handle_sigabrt);

    /* TODO Come up with more informative arguments */
    if (argc > 1 && argv[1][0] == 's') {
        run_slow_tests = 1;
    }
    system("rm -f tests/data/*.dat");

    pid_test_server = fork();
    if (pid_test_server < 0) {
        perror("fork filer");
        return 1;
    } else if (pid_test_server == 0) {
        char *fling_argv[] = {"../bin/fling", "serve", NULL};
        printf("Running fling in server mode...\n");
        if (chdir("tests/data") != 0) {
            perror("chdir");
            return 1;
        }
        execvp("../../bin/fling", fling_argv);
        perror("execvp fling");
        exit(1);
    } else {
        WAITABIT();
        run_tests();
        _cleanup();
        if (test_failures > 0) {
            printf("\n%d test(s) failed!\n", test_failures);
            return 1;
        }
        printf("Done testing - all tests passed!\n");
    }
    return 0;
}
