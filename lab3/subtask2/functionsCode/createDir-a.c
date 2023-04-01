#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

void createDir(char* dirName) {
    if (mkdir(dirName, S_IROTH | S_IWOTH | S_IXOTH | S_IRUSR | S_IWUSR | 
                                S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP) != 0) {
        perror("mkdir() ");
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
    createDir(dirName);

    return 0;
}