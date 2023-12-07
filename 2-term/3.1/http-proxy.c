
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>
#include <signal.h>
#include <errno.h>

#include "http-proxy.h"

// add signal hadler for ^C ^'\'

int open_proxy_listening_socket(int listening_socket_fd, int port);
int handle_connect_request(int listening_socket_fd); // check cache if not present then creates new thread

int proxy_run(int port) {
    int listening_socket_fd = -1;
    int fds_array_length = 1 + MAX_CONNECTIONS * 2;
    struct pollfd *fds_array; 
    // proxy cache

    fprintf(stdout, "proxy is starting ...\n");

    if (open_proxy_listening_socket(listening_socket_fd, port) == -1) {
        fprintf(stderr, "error :: cannot open proxy listening socket\n");
        proxy_stop();
        return -1;
    }

    fds_array = malloc(fds_array_length * sizeof(struct pollfd));
    memset(fds_array, -1, fds_array_length * sizeof(struct pollfd));

    fds_array[0].fd = listening_socket_fd;
    fds_array[0].events = POLLIN;

    fprintf(stdout, "proxy starts listening for connections ...\n");

    while (1) {
        if (poll(fds_array, fds_array_length, -1) == -1) {
            fprintf(stderr, "error :: poll() :: %s\n", strerror(errno));
            proxy_stop();
            return -1;
        }

        if (fds_array[0].revents & POLLIN) {
            if (handle_connect_request(listening_socket_fd) == -1) {
                fprintf(stderr, "error :: cannot handle connection request\n");
                proxy_stop();
                return -1;
            }
        }
    }
}

int open_proxy_listening_socket(int listening_socket_fd, int port) {
    struct sockaddr_in  listening_socket_addr;
    listening_socket_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    listening_socket_addr.sin_family = AF_INET;
    listening_socket_addr.sin_port = htons(port);

    listening_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listening_socket_fd == -1) {
        fprintf(stderr, "error :: socket() :: %s\n", strerror(errno));
        return -1;
    }

    if (bind(listening_socket_fd, (struct sockaddr *) &listening_socket_addr, sizeof(listening_socket_addr))) {
        fprintf(stderr, "error :: bind() :: %s\n", strerror(errno));
        if (errno == EADDRINUSE) {
            fprintf(stderr, "bind() :: port is already in use\n");
        }
        return -1;
    }

    if (listen(listening_socket_fd, MAX_CONNECTIONS)) {
        fprintf(stderr, "error :: listen() :: %s\n", strerror(errno));
        return -1;
    }

    return 0;
}

int handle_connect_request(int listening_socket_fd) {
    return 0;
}

int proxy_stop() {
    return 0;
}