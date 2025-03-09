#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "const.h"
#include "file.h"
#include "client.h"
#include "server.h"
#include "progress.h"
#include "receiver.h"
#include "sender.h"
#include "version.h"

static void print_usage(const char *progname)
{
    printf("fling %s. Usage:\n", FLING_VERSION);
    printf("  %s serve [port]               Start in server mode "
           "(default port: " DEFAULT_PORT_STR ")\n", progname);
    printf("  %s send <file> <host> [port]  Send a file "
           "(default port: " DEFAULT_PORT_STR ")\n", progname);
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "serve") == 0) {
        /* Server mode - receive files */
        int port = DEFAULT_PORT;
        if (argc > 2) {
            port = atoi(argv[2]);
            if (port == 0) {
                printf("Incorrect port number '%s'\n", argv[2]);
                return 1;
            }
        }
        return exec_receiver(port);
    }

    if (strcmp(argv[1], "send") == 0) {
        /* Client mode - send file */
        if (argc < 4) {
            printf("Error: Missing file or host arguments for send command\n");
            print_usage(argv[0]);
            return 1;
        }

        char *filename = argv[2];
        const char *host = argv[3];
        const char *port = DEFAULT_PORT_STR;
        
        if (argc > 4) {
            port = argv[4];
        }
        
        return exec_sender(filename, host, port);
    }

    printf("Unknown command: %s\n", argv[1]);
    print_usage(argv[0]);
    return 1;
}
