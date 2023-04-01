#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

void createSymLink(char* fileName) {
    char* newFileName = malloc(255);
    int i = 0;
    while (fileName[i] != '\0') {
        newFileName[i] = fileName[i];
        i++;
    }
    strcat(newFileName, "-link");
    if (symlink(fileName, newFileName) == -1) {
        perror("symlink() ");
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
    createSymLink(fileName);

    return 0;
}