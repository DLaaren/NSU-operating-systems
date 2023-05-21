#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define addr "127.0.0.1"

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Should be: %s <port>\n", argv[0]);
        exit(1);
    }

    unsigned int port = atoi(argv[1]);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket()");
        exit(2);
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock, (struct sockaddr*)&server, sizeof(server)) == -1) {
        perror("bind()");
        exit(3);
    }

    if (listen(sock,1) != 0) {
        perror("listen()");
        exit(4);
    }

    struct sockaddr_in client;
    int len = sizeof(client);

    while (1) {
        int clientsock = accept(sock, (struct sockaddr*)&client, &len);
        if (clientsock == -1) {
            perror("accept()");
            exit(5);
        }    

        char buf[64];
        if (recv(clientsock, buf, sizeof(buf), 0) == -1) {
            perror("recv()");
            exit(6);
        }

        printf("Received your greets!\n");

        char reply[] = "Message from server!\n";
        if (send(clientsock, reply, sizeof(reply), 0) == -1) {
            perror("send()");
            exit(7);
        }

        close(clientsock);
    }

    close(sock);
    return 0;
}