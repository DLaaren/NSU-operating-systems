#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <glob.h>
#include <string.h>

void readDir(char* dirName) {
    glob_t globBuf;
    strcat(dirName,"*");
    if (glob(dirName, 0, NULL, &globBuf) != 0) {
        perror("glob() ");
        globfree(&globBuf);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < globBuf.gl_pathc; i++) {
        printf("%s\n", globBuf.gl_pathv[i]);
    }

    globfree(&globBuf);
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
    readDir(dirName);

    return 0;
}