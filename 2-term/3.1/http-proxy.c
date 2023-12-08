#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>

#include "http-proxy.h"
#include "picohttpparser.h"

#define BUFFER_SIZE 2048
#define DEFAULT_PORT 80

// add signal hadler for ^C ^'\'

int open_proxy_listening_socket(int listening_socket_fd, int port);
int parse_http_request(char *buffer, int buffer_len, char *ip, int ip_length, char *port);
void *handle_connect_request(int client_socket_fd);

int proxy_run(int port) {
    int listening_socket_fd = -1;
    // proxy cache

    fprintf(stdout, "proxy is starting ...\n");

    listening_socket_fd = open_proxy_listening_socket(listening_socket_fd, port);
    if (listening_socket_fd == -1) {
        fprintf(stderr, "error :: cannot open proxy listening socket\n");
        proxy_stop(listening_socket_fd);
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

    if (setsockopt(listening_socket_fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0) {
        fprintf(stderr, "error :: setsockopt() :: %s\n", strerror(errno));
        return -1;
    }

    if (bind(listening_socket_fd, (struct sockaddr *) &proxy_addr, sizeof(proxy_addr))  ==  -1) {
        fprintf(stderr, "error :: bind() :: %s\n", strerror(errno));
        return -1;
    }

    if (listen(listening_socket_fd, MAX_CONNECTIONS)  ==  -1) {
        fprintf(stderr, "error :: listen() :: %s\n", strerror(errno));
        return -1;
    }

    return listening_socket_fd;
}

void *handle_connect_request(int client_socket_fd) {
    int err = 0;
    int host_socket_fd = -1;
    struct sockaddr_in host_address;
    char host_ip[16] = {0};
    char host_port[8] = {0};
    int bytes_read = 0; 
    int bytes_written = 0;
    char buffer[BUFFER_SIZE] = {0};

    fprintf(stdout, "got new connection request on socket %d\n", client_socket_fd);

    // read from client
    bytes_read = read(client_socket_fd, buffer, BUFFER_SIZE);
    if (bytes_read == -1) {
        fprintf(stderr, "error :: read() :: %s\n", strerror(errno));
        close(client_socket_fd);
        return NULL;
    }
    else if (bytes_read == 0) {
        fprintf(stderr, "warning :: connection on socket %d has been lost\n", client_socket_fd);
        close(client_socket_fd);
        return NULL;
    }

    if (parse_http_request(buffer, bytes_read, host_ip, sizeof(host_ip), host_port) == -1) {
        fprintf(stderr, "error :: parse_http_request()\n");
        close(client_socket_fd);
        return NULL;
    }

    fprintf(stdout, "trying connect to host :: %s:%s\n", host_ip, host_port);
    

    host_socket_fd = socket(AF_INET, SOCK_STREAM, 0); 
    if (host_socket_fd == -1) {
        fprintf(stderr, "error :: socket() :: %s\n", strerror(errno));
        close(client_socket_fd);
        return NULL;
    }

    host_address.sin_family = AF_INET;

    if (inet_pton(AF_INET, host_ip, &(host_address.sin_addr)) == -1) {
        fprintf(stderr, "error :: inet_pton() :: %s\n", strerror(errno));
        close(client_socket_fd);
        close(host_socket_fd);
        return NULL;
    }

    if (host_port[0] != '\0') {
        host_address.sin_port = htons(host_port);
    } else {
        host_address.sin_port = htons(DEFAULT_PORT);
    }

    if (connect(host_socket_fd, (struct sockaddr *) &host_address, sizeof(host_address)) == -1) {
        fprintf(stderr, "error :: connect() :: %s\n", strerror(errno));
        close(client_socket_fd);
        close(host_socket_fd);
        return NULL;
    }

    fprintf(stdout, "connection has been successful\n");

    // write to host
    bytes_written = write(host_socket_fd, buffer, bytes_read);
    if (bytes_written == -1) {
        fprintf(stderr, "error :: write() :: %s\n", strerror(errno));
        close(client_socket_fd);
        close(host_socket_fd);
        return NULL;
    }
    memset(buffer, 0, BUFFER_SIZE);

    // read from host
    bytes_read = read(host_socket_fd, buffer, BUFFER_SIZE);
    if (bytes_read == -1) {
        fprintf(stderr, "error :: read() :: %s\n", strerror(errno));
        close(client_socket_fd);
        close(host_socket_fd);
        return NULL;
    }
    else if (bytes_read == 0) {
        fprintf(stderr, "warning :: connection on socket %d has been lost\n", client_socket_fd);
        close(client_socket_fd);
        close(host_socket_fd);
        return NULL;
    }

    // write response to client
    bytes_written = write(client_socket_fd, buffer, bytes_read);
    if (bytes_written == -1) {
        fprintf(stderr, "error :: write() :: %s\n", strerror(errno));
        close(client_socket_fd);
        close(host_socket_fd);
        return NULL;
    }

    close(client_socket_fd);
    close(host_socket_fd);
    return NULL;
}

int parse_http_request(char *buffer, int buffer_len, char *ip, int ip_length, char *port) {
    int pret;
    struct phr_header headers[100];
    size_t num_headers = 100;
    char *method, *path;
    int minor_version;
    size_t method_len, path_len;
    char tmp[16];
    struct addrinfo hints;
    struct addrinfo* result, * rp;

    pret = phr_parse_request(buffer, buffer_len, &method, &method_len, &path, &path_len,
                             &minor_version, headers, &num_headers, 0);
    if (pret == -1) {
        return -1;
    }

    size_t i;
    for (i = 0; i < num_headers; i++) {
        if (headers[i].name[0] == 'H' &&
            headers[i].name[1] == 'o' &&
            headers[i].name[2] == 's' &&
            headers[i].name[3] == 't') {
            break;
        }
    }

    sprintf(tmp, "%.*s", (int)headers[i].value_len, headers[i].value);

    printf("tmp :: %s\n", tmp);

    if (strstr(tmp, ":") != NULL) {
        strcpy(ip, strtok(tmp, ":"));
        strcpy(port, strtok(NULL, ":"));
    }
    else {
        strcpy(ip, tmp);
    }

    printf("ip %s port %s\n", ip, port);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(ip, NULL, &hints, &result) == -1) {
        fprintf(stderr, "error :: geaddrinfo() :: %s\n", strerror(errno));
        return -1;
    }

    memset(ip, 0, ip_length);

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        struct sockaddr_in* ipv4 = (struct sockaddr_in*)rp->ai_addr;
        void* addr = &(ipv4->sin_addr);

        if (inet_ntop(AF_INET, addr, ip, ip_length) != NULL) {
            freeaddrinfo(result);
            return 0;
        }
        else {
            fprintf(stderr, "error :: inet_ntop() :: %s\n", strerror(errno));
            return -1;
        }
    }

    freeaddrinfo(result);
    return -1;
}

int proxy_stop(int listening_socket_fd) {
    close(listening_socket_fd);
    return 0;
}