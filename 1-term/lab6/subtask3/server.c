#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    int len;
    int server_socket;
    int client_socket;
    char* domain_socket_file = "./domain_socket_file";
    unlink(domain_socket_file);

    server_socket = socket(AF_UNIX /*local communication*/, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("socket()");
        return -1;
    }

    struct sockaddr_un server_socketaddr;
    memset(&server_socketaddr, 0, sizeof(struct sockaddr_un));
    server_socketaddr.sun_family = AF_UNIX;
    strcpy(server_socketaddr.sun_path, domain_socket_file);

    int err = bind(server_socket, (struct sockaddr*) &server_socketaddr, sizeof(server_socketaddr));
    if (err == -1) {
        perror("bind()");
        return -1;
    }

    err = listen(server_socket, 20);
    if (err == -1) {
        perror("listen()");
        return -1;
    }

    while(1) {
        client_socket = accept(server_socket, NULL, NULL);
        if (client_socket == -1) {
            perror("accept()");
            return -1;
        }

        unsigned int num;
        err = read(client_socket, &num, sizeof(num));
        if (err == -1) {
            perror("read()");
        }

        printf("get number %u\n", num);

        err = write(client_socket, &num, sizeof(num));
        if (err == -1) {
            perror("write()");
        }
        close (client_socket);
    }

    close(server_socket);
    unlink(domain_socket_file);
    return 0;
}