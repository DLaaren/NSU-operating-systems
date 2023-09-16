#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char** argv) {
    if (argc <= 2) {
        printf("Not enough arguments\n");
        exit(EXIT_FAILURE);
    }
    char* funcName = calloc(255, 1);
    strcat(funcName, "./functions/");
    strcat(funcName, argv[1]);
    printf("path to the function: %s\n", funcName);
    printf("argument to the function: %s\n", argv[2]);

    if (argc == 3) {
        if (execl(funcName, argv[2], argv[2], (char*)NULL) == -1) {
            perror("execl() ");
            free(funcName);
            exit(EXIT_FAILURE);
        }
    }
    if (argc == 4) {
        if (execl(funcName, argv[2], argv[2], argv[3], (char*)NULL) == -1) {
            perror("execl() ");
            free(funcName);
            exit(EXIT_FAILURE);
        }
    }
    free(funcName);
    return 0;
}