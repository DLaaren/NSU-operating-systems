#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void createFile(char* fileName) {
    if (creat(fileName, S_IROTH | S_IWOTH | S_IXOTH | S_IRUSR | S_IWUSR | 
                        S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP) == -1) {
        perror("creat() ");
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
    createFile(fileName);

    return 0;
}