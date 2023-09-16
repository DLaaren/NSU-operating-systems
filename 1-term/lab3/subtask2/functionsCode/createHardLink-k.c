#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

void createHardLink(char* fileName) {
    char* newFileName = malloc(256);
    int i = 0;
    while (fileName[i] != '\0') {
        newFileName[i] = fileName[i];
        i++;
    }
    strcat(newFileName, "-hardlink");
    if (link(fileName, newFileName) == -1) {
        perror("link() ");
        free(newFileName);
        exit(EXIT_FAILURE);
    }
    free(newFileName);
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
    createHardLink(fileName);

    return 0;
}