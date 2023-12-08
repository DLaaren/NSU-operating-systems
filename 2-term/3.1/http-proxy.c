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

#define BUFFER_SIZE 2048

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
        pthread_t tid;
        pthread_attr_t attr;
        int client_socket_fd;
        struct sockaddr_in client_address;
        socklen_t client_address_length = sizeof(client_address);
        client_address.sin_family = AF_INET;

        client_socket_fd = accept(listening_socket_fd, (struct sockaddr *) &client_address, &client_address_length);
        if (client_socket_fd == -1) {
            fprintf(stderr, "error :: accept() :: %s\n", strerror(errno));
            continue;
        }

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
        return -1;
    }

    if (listen(listening_socket_fd, MAX_CONNECTIONS)) {
        fprintf(stderr, "error :: listen() :: %s\n", strerror(errno));
        return -1;
    }

    return listening_socket_fd;
}

void *handle_connect_request(int client_socket_fd) {
    int host_socket_fd = -1;
    struct sockaddr_in host_address;
    int bytes_read = 0; 
    int bytes_written = 0;
    char buffer[BUFFER_SIZE] = {0};

    fprintf(stdout, "got new connection request on socket %d\n", client_socket_fd);

    // read from client
    bytes_read = read(client_socket_fd, buffer, BUFFER_SIZE);
    if (bytes_read == -1) {
        fprintf(stderr, "error :: read() :: %s\n", strerror(errno));
        return NULL;
    }
    else if (bytes_read == 0) {
        fprintf(stderr, "warning :: connection on socket %d has been lost\n", client_socket_fd);
        return NULL;
    }

    // parse message


    // connect ot host 
    host_socket_fd = socket(AF_INET, SOCK_STREAM, 0); 
    if (host_socket_fd == -1) {
        fprintf(stderr, "error :: socket() :: %s\n", strerror(errno));
        return NULL;
    }
    host_address.sin_family = AF_INET;
    if (inet_pton(AF_INET, /* host address */, &(host_address.sin_addr)) == -1) {
        fprintf(stderr, "error :: inet_pton() :: %s\n", strerror(errno));
        return NULL;
    }
    host_address.sin_port = htons(/* host port */);

    if (connect(host_socket_fd, (struct sockaddr *) &host_address, sizeof(host_address)) == -1) {
        fprintf(stderr, "error :: connect() :: %s\n", strerror(errno));
        return NULL;
    }

    // write to host
    bytes_written = write(host_socket_fd, buffer, bytes_read);
    if (bytes_written == -1) {
        fprintf(stderr, "error :: write() :: %s\n", strerror(errno));
        return NULL;
    }
    memset(buffer, 0, BUFFER_SIZE);

    // read from host
    bytes_read = read(host_socket_fd, buffer, BUFFER_SIZE);
    if (bytes_read == -1) {
        fprintf(stderr, "error :: read() :: %s\n", strerror(errno));
        return NULL;
    }
    else if (bytes_read == 0) {
        fprintf(stderr, "warning :: connection on socket %d has been lost\n", client_socket_fd);
        return NULL;
    }

    // write response to client
    bytes_written = write(client_socket_fd, buffer, bytes_read);
    if (bytes_written == -1) {
        fprintf(stderr, "error :: write() :: %s\n", strerror(errno));
        return NULL;
    }

    close(client_socket_fd);
    close(host_socket_fd);
    return NULL;
}

int proxy_stop() {
    return 0;
}