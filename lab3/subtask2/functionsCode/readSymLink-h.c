#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void readSymLink(char* linkName) {
    char* buf = malloc(255);
    if (readlink(linkName, buf, 255) == -1) {
        perror("readlink() ");
        free(buf);
        exit(EXIT_FAILURE);
    }
    printf("%s\n", buf);
    free(buf);
}

int main(int argc, char** argv) {
    if (argc == 1) {
        printf("Enter an absolute path to your link\n");
        exit(EXIT_FAILURE);
    }

    if (argc >= 3) {
        printf("Too much arguments\n");
        exit(EXIT_FAILURE);
    }

    char* linkName = argv[1];
    readSymLink(linkName);

    return 0;
}