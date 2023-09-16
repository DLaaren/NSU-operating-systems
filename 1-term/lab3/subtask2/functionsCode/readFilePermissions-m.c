#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

void changePermissions(char* fileName) {
    struct stat fileStat;
    if (stat(fileName, &fileStat) != 0) {
        perror("stat() ");
        exit(EXIT_FAILURE);
    }
    printf("amout of hard links is %d\n",  fileStat.st_nlink);
    printf("permissions bits are %o\n", fileStat.st_mode);
    //owner permissions
    if (fileStat.st_mode & S_IRUSR) {
        printf("r");
    } else {
        printf("-");
    }
    if (fileStat.st_mode & S_IWUSR) {
        printf("w");
    } else {
        printf("-");
    }
    if (fileStat.st_mode & S_IXUSR) {
        printf("x");
    } else {
        printf("-");
    }
    //group permissions
    if (fileStat.st_mode & S_IRGRP) {
        printf("r");
    } else {
        printf("-");
    }
    if (fileStat.st_mode & S_IWGRP) {
        printf("w");
    } else {
        printf("-");
    }
    if (fileStat.st_mode & S_IWGRP) {
        printf("x");
    } else {
        printf("-");
    }
    //others
    if (fileStat.st_mode & S_IROTH) {
        printf("r");
    } else {
        printf("-");
    }
    if (fileStat.st_mode & S_IWOTH) {
        printf("w");
    } else {
        printf("-");
    }
    if (fileStat.st_mode & S_IXOTH) {
        printf("x");
    } else {
        printf("-");
    }
    printf("\n");
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
    changePermissions(fileName);

    return 0;
}