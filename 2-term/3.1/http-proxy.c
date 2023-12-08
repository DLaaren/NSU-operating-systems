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
#include <pthread.h>

#include "http-proxy.h"

// add signal hadler for ^C ^'\'

int open_proxy_listening_socket(int listening_socket_fd, int port);
void *handle_connect_request(int client_socket_fd); // check cache if not present then creates new thread

int proxy_run(int port) {
    int listening_socket_fd = -1;
    // proxy cache

    fprintf(stdout, "proxy is starting ...\n");

    listening_socket_fd = open_proxy_listening_socket(listening_socket_fd, port);
    if (listening_socket_fd == -1) {
        fprintf(stderr, "error :: cannot open proxy listening socket\n");
        proxy_stop();
        return -1;
    }

    fprintf(stdout, "proxy starts listening for connections ...\n");

    while (1) {
        int client_socket_fd;
        struct sockaddr_in client_address;
        socklen_t client_address_length = sizeof(client_address);
        client_address.sin_family = AF_INET;

        client_socket_fd = accept(listening_socket_fd, (struct sockaddr *) &client_address, &client_address_length);
        if (client_socket_fd == -1) {
            fprintf(stderr, "error :: accept() :: %s\n", strerror(errno));
            continue;
        }
        else {
            pthread_t tid;
            pthread_attr_t attr;

            pthread_attr_init(&attr);

            if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) == -1) {
                pthread_attr_destroy(&attr);
                fprintf(stderr, "error :: pthread_attr_setdetachstate() :: %s\n", strerror(errno));
                continue;
            }

            if (pthread_create(&tid, &attr, handle_connect_request, client_socket_fd) == -1) {
                pthread_attr_destroy(&attr);
                fprintf(stderr, "error :: pthread_create() :: %s\n", strerror(errno));
                continue;
            }

            pthread_attr_destroy(&attr);
        }
    }
}

int open_proxy_listening_socket(int listening_socket_fd, int port) {
    struct sockaddr_in proxy_addr;
    proxy_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    proxy_addr.sin_family = AF_INET;
    proxy_addr.sin_port = htons(port);

    listening_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listening_socket_fd == -1) {
        fprintf(stderr, "error :: socket() :: %s\n", strerror(errno));
        return -1;
    }

    if (bind(listening_socket_fd, (struct sockaddr *) &proxy_addr, sizeof(proxy_addr))) {
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

    return listening_socket_fd;
}

void *handle_connect_request(int client_socket_fd) {
    fprintf(stdout, "got new connection request on socket %d\n", client_socket_fd);

    

    close(client_socket_fd);
    return NULL;
}

int proxy_stop() {
    return 0;
}