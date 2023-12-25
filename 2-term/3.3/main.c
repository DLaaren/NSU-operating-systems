#include <stdio.h>
#include <stdlib.h>

#include "http-proxy.h"

int main(int argc, char **argv) {
    int port;
    if (argc < 2) {
        fprintf(stderr, "please enter a port\n");
        exit(EXIT_FAILURE);
    }
    else {
        port = atoi(argv[1]);
        if (port < 0) {
            fprintf(stderr, "please enter a non-negative value for port\n");
            exit(EXIT_FAILURE);
        }
        if (port > 65535) {
            fprintf(stderr, "please enter a 2-byte long value for port\n");
            exit(EXIT_FAILURE);
        }
    }

    // printf("%d\n", port);

    proxy_run(port);

    return 0;
}