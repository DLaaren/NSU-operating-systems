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

int main() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("socket()");
        exit(1);
    } 

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = 0; //use any available port
    server.sin_addr.s_addr = inet_addr(addr);

    if (bind(sock, (struct sockaddr*)&server, sizeof(server)) == -1) {
        perror("bind()");
        exit(2);
    }

    //find out port
    int len = sizeof(server);
    if (getsockname(sock,(struct sockaddr*)&server, &len) == -1) {
        perror("getsockname()");
        exit(3);
    }

    printf("Address is %s :: Port is %d\n", addr, ntohs(server.sin_port));

    while(1) {
        char buf[64];
        struct sockaddr_in client;
        int client_address_size = sizeof(client);
        if (recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr*)&client, &client_address_size) == -1) {
            perror("recvfrom()");
            exit(4);
        }

        printf("Received message \"%s\" from domain %s port %d internet address %s\n", 
            buf, 
            (client.sin_family == AF_INET ? "AF_INET" : "UNKNOWN"),
            ntohs(client.sin_port),
            inet_ntoa(client.sin_addr));

    }
    close(sock);

    return 0;
}