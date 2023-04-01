#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void removeLink(char* linkName) {
    if (unlink(linkName) == -1) {
        perror("unlink() ");
        exit(EXIT_FAILURE);
    }
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
    removeLink(linkName);

    return 0;
}