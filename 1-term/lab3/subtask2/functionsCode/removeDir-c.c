#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void removeDir(char* dirName) {
    if (rmdir(dirName) != 0) {
        perror("rmdir() ");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char** argv) {
    if (argc == 1) {
        printf("Enter an absolute path to your directory\n");
        exit(EXIT_FAILURE);
    }

    if (argc >= 3) {
        printf("Too much arguments\n");
        exit(EXIT_FAILURE);
    }

    char* dirName = argv[1];
    removeDir(dirName);

    return 0;
}