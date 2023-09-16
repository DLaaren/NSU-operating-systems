#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv) {
    // argv[1] is internet address of server argv[2] is port of server.
    if (argc != 3) {
        printf("Should be: %s <host address> <port>\n", argv[0]);
        exit(1);
    }

    unsigned int port = htons(atoi(argv[2]));

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("socket()");
        exit(2);
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = port;
    server.sin_addr.s_addr = inet_addr(argv[1]);

    char buf[] = "Hello from client!";

    if (sendto(sock, buf, sizeof(buf), 0, (struct sockaddr*)&server, sizeof(server)) == -1) {
        perror("sendto()");
        exit(3);
    }

    close(sock);

    return 0;
}