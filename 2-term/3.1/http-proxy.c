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

#define LOG(...) \
    fprintf(stdout, __VA_ARGS__);
#define ELOG(...) \
    fprintf(stderr, __VA_ARGS__);

#define MAX_CONNECTIONS 10
#define BUFFER_SIZE 2048
#define DEFAULT_PORT "80"

// add signal hadler for ^C ^'\'

int open_proxy_listening_socket(int listening_socket_fd, int port);
int parse_http_request(char *buffer, int buffer_len, char *ip, int ip_length, char *port);
void *handle_connect_request(int client_socket_fd);

int proxy_run(int port) {
    int listening_socket_fd = -1;
    // proxy cache

    LOG("proxy is starting ...\n");

    listening_socket_fd = open_proxy_listening_socket(listening_socket_fd, port);
    if (listening_socket_fd == -1) {
        ELOG("error :: cannot open proxy listening socket\n");
        proxy_stop(listening_socket_fd);
        return -1;
    }

    LOG("proxy starts listening for connections ...\n");

    while (1) {
        pthread_t tid;
        pthread_attr_t attr;
        int client_socket_fd;
        struct sockaddr_in client_address;
        socklen_t client_address_length = sizeof(client_address);
        client_address.sin_family = AF_INET;

        client_socket_fd = accept(listening_socket_fd, (struct sockaddr *) &client_address, &client_address_length);
        if (client_socket_fd == -1) {
            ELOG("error :: accept() :: %s\n", strerror(errno));
            continue;
        }

        pthread_attr_init(&attr);

        if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) == -1) {
            pthread_attr_destroy(&attr);
            close(client_socket_fd);
            ELOG("error :: pthread_attr_setdetachstate() :: %s\n", strerror(errno));
            continue;
        }

        if (pthread_create(&tid, &attr, handle_connect_request, client_socket_fd) == -1) {
            pthread_attr_destroy(&attr);
            close(client_socket_fd);
            ELOG("error :: pthread_create() :: %s\n", strerror(errno));
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
        ELOG("error :: socket() :: %s\n", strerror(errno));
        return -1;
    }

    if (setsockopt(listening_socket_fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0) {
        ELOG("error :: setsockopt() :: %s\n", strerror(errno));
        return -1;
    }

    if (bind(listening_socket_fd, (struct sockaddr *) &proxy_addr, sizeof(proxy_addr))  ==  -1) {
        ELOG("error :: bind() :: %s\n", strerror(errno));
        return -1;
    }

    if (listen(listening_socket_fd, MAX_CONNECTIONS)  ==  -1) {
        ELOG("error :: listen() :: %s\n", strerror(errno));
        return -1;
    }

    return listening_socket_fd;
}

// TODO
// добавить signal handlers
// добавить кэш


void *handle_connect_request(int client_socket_fd) {
    int host_socket_fd = -1;
    struct sockaddr_in host_address;
    char host_ip[16] = {0};
    char host_port[8] = {0};
    int bytes_read = 0; 
    int bytes_written = 0;
    char *buffer = malloc(BUFFER_SIZE * sizeof(char));

    LOG("got new connection request on socket %d\n", client_socket_fd);

    // read from client
    bytes_read = read(client_socket_fd, buffer, BUFFER_SIZE);
    if (bytes_read == -1) {
        ELOG("error :: read() :: %s\n", strerror(errno));
        close(client_socket_fd);
        free(buffer);
        return NULL;
    }
    else if (bytes_read == 0) {
        ELOG("warning :: connection on socket %d has been lost\n", client_socket_fd);
        close(client_socket_fd);
        free(buffer);
        return NULL;
    }

    if (parse_http_request(buffer, bytes_read, host_ip, sizeof(host_ip), host_port) == -1) {
        ELOG("error :: parse_http_request()\n");
        close(client_socket_fd);
        free(buffer);
        return NULL;
    }

    LOG("trying connect to host :: %s:%s on socket %d\n", host_ip, host_port, client_socket_fd);
    
    host_socket_fd = socket(AF_INET, SOCK_STREAM, 0); 
    if (host_socket_fd == -1) {
        ELOG("error :: socket() :: %s\n", strerror(errno));
        close(client_socket_fd);
        free(buffer);
        return NULL;
    }

    host_address.sin_family = AF_INET;

    if (inet_pton(AF_INET, host_ip, &(host_address.sin_addr)) == -1) {
        ELOG("error :: inet_pton() :: %s\n", strerror(errno));
        close(client_socket_fd);
        close(host_socket_fd);
        free(buffer);
        return NULL;
    }

    host_address.sin_port = htons(atoi(host_port));

    if (connect(host_socket_fd, (struct sockaddr *) &host_address, sizeof(host_address)) == -1) {
        ELOG("error :: connect() :: %s\n", strerror(errno));
        close(client_socket_fd);
        close(host_socket_fd);
        free(buffer);
        return NULL;
    }

    LOG("connection to host %s:%s on socket %d has been successful\n",  host_ip, host_port, client_socket_fd);

    // write to host
    bytes_written = write(host_socket_fd, buffer, bytes_read);
    if (bytes_written == -1) {
        ELOG("error :: write() :: %s\n", strerror(errno));
        close(client_socket_fd);
        close(host_socket_fd);
        free(buffer);
        return NULL;
    }

    do {
        // read from host
        bytes_read = read(host_socket_fd, buffer, BUFFER_SIZE);
        if (bytes_read == -1) {
            ELOG("error :: read() :: %s\n", strerror(errno));
            close(client_socket_fd);
            close(host_socket_fd);
            free(buffer);
            return NULL;
        }
        else if (bytes_read == 0) {
            break;
        }

        // write response to client
        bytes_written = write(client_socket_fd, buffer, bytes_read);
        if (bytes_written == -1) {
            ELOG("error :: write() :: %s\n", strerror(errno));
            close(client_socket_fd);
            close(host_socket_fd);
            free(buffer);
            return NULL;
        }

    } while (bytes_read > 0);

    LOG("closing connection on socket %d\n", client_socket_fd)

    close(client_socket_fd);
    close(host_socket_fd);
    free(buffer);
    return NULL;
}

int parse_http_request(char *buffer, int buffer_len, char *ip, int ip_length, char *port) {
    int err, pret;
    struct phr_header headers[100];
    size_t num_headers = 100;
    char *method, *path;
    int minor_version;
    size_t method_len, path_len;
    char tmp[256];
    struct addrinfo hints;
    struct addrinfo *result;

    pret = phr_parse_request(buffer, buffer_len, &method, &method_len, &path, &path_len,
                             &minor_version, headers, &num_headers, 0);
    if (pret == -1) {
        ELOG("error :: phr_parse_request()\n");
        return -1;
    }

    // change to strcmp

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

    // LOG("CHECK :: %s\n\n", buffer);

    if (strstr(tmp, ":") != NULL) {
        strcpy(ip, strtok(tmp, ":"));
        strcpy(port, strtok(NULL, ":"));
    }
    else {
        strcpy(ip, tmp);
        strcpy(port, DEFAULT_PORT);
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    LOG("trying to resolve ip :: %s\n", ip);

    if ((err = getaddrinfo(ip, "http", &hints, &result)) != 0) {
        ELOG("error :: getaddrinfo() :: %s :: %s\n", strerror(errno), gai_strerror(err));
        return -1;
    }

    memset(ip, 0, ip_length);    

    for (struct addrinfo *rp = result; rp != NULL; rp = rp->ai_next) {
        struct sockaddr_in* curr_addr = (struct sockaddr_in*)rp->ai_addr;
        void* addr = &(curr_addr->sin_addr);

        if (inet_ntop(AF_INET, addr, ip, ip_length) != NULL) {
            freeaddrinfo(result);
            return 0;
        }
        else {
            ELOG("error :: inet_ntop() :: %s\n", strerror(errno));
            freeaddrinfo(result);
            return -1;
        }
    }

    freeaddrinfo(result);
    ELOG("error :: cannot resolve domain name\n");
    return -1;
}

int proxy_stop(int listening_socket_fd) {
    close(listening_socket_fd);
    return 0;
}