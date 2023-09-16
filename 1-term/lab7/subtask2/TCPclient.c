#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define h_addr  h_addr_list[0]

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("Shoud be: %s <hostname> <port>\n", argv[0]);
        exit(1);
    }

    struct hostent* hostnm = gethostbyname(argv[1]);
    if (hostnm == (struct hostent *) 0) {
        perror("gethostbyname()");
        exit(2);
    }

    unsigned int port = atoi(argv[2]);

    char buf[] = "Hello from client!\n";
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = *((unsigned long *)hostnm->h_addr);

    int sock = socket(AF_INET, SOCK_STREAM, 0); 
    if (sock == -1) {
        perror("socket()");
        exit(3);
    }

     if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("connect()");
        exit(4);
    }

    if (send(sock, buf, sizeof(buf), 0) == -1) {
        perror("send()");
        exit(5);
    }

    printf("Send message :: %s\n", buf);

    if (recv(sock, buf, sizeof(buf), 0) == -1) {
        perror("recv()");
        exit(6);
    }

    printf("Got message :: %s\n", buf);

    close(sock);

    return 0;
}