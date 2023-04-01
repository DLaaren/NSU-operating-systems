#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void removeFile(char* fileName) {
    if (unlink(fileName) == -1) {
        perror("unlink() ");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char** argv) {
    if (argc == 1) {
        printf("Enter an absolute path to your file\n");
        exit(EXIT_FAILURE);
    }

    if (argc >= 3) {
        printf("Too much arguments\n");
        exit(EXIT_FAILURE);
    }

    char* fileName = argv[1];
    removeFile(fileName);

    return 0;
}