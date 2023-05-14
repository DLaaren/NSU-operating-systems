#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

int main() {
    srand(time(NULL));
    int client_socket;
    char* domain_socket_file = "./domain_socket_file";

    client_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("socket()");
        return -1;
    } 

    struct sockaddr_un client_socketaddr;
    memset(&client_socketaddr, 0, sizeof(client_socketaddr));
    client_socketaddr.sun_family = AF_UNIX;
    strcpy(client_socketaddr.sun_path, domain_socket_file);

    int err = connect(client_socket, (struct sockaddr*) &client_socketaddr, sizeof(client_socketaddr));
    if (err == -1) {
        perror("connect() :: server is shutted down");
        return -1;
    }

    unsigned int num = rand() % 100;
    err = write(client_socket, &num, sizeof(num));
    if (err == -1) {
        perror("write()");
    }

    unsigned int res;
    err = read(client_socket, &res, sizeof(res));
    if (err == -1) {
        perror("read()");
    }
    if (num != res) {
        fprintf(stderr, "wrong returned number");
    } else {
        printf("correct %u\n", res);
    }

    close(client_socket);
    return 0;

}