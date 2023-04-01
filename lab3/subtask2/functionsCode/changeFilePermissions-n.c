#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>

int parsePermissions(char* permissions) {
    int res = 0;
    if (permissions[0] == 'r') {
        res |= S_IRUSR;
    }
    if (permissions[1] == 'w') {
        res |= S_IWUSR;
    }
    if (permissions[2] == 'x') {
        res |= S_IXUSR;
    }
    if (permissions[3] == 'r') {
        res |= S_IRGRP;
    }
    if (permissions[4] == 'w') {
        res |= S_IWGRP;
    }
    if (permissions[5] == 'x') {
        res |= S_IXGRP;
    }
    if (permissions[6] == 'r') {
        res |= S_IROTH;
    }
    if (permissions[7] == 'w') {
        res |= S_IWOTH;
    }
    if (permissions[8] == 'x') {
        res |= S_IXOTH;
    }
    return res;
}

void readFilePermissions(char* fileName, int permissions) {
    if (chmod(fileName, permissions) != 0) {
        perror("chmod() ");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char** argv) {
    if (argc == 1) {
        printf("Enter an absolute path to your file\n");
        exit(EXIT_FAILURE);
    }

    if (argc == 2) {
        printf("Enter a permissions\n");
        exit(EXIT_FAILURE);
    }

    if (argc >= 4) {
        printf("Too much arguments\n");
        exit(EXIT_FAILURE);
    }

    char* fileName = argv[1];
    char* permissions = argv[2];
    int octalFormPermissions = parsePermissions(permissions);
    readFilePermissions(fileName, octalFormPermissions);

    return 0;
}