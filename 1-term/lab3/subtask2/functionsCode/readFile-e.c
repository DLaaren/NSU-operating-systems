#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void readFile(char* fileName) {
    char* buf = malloc(1);
    size_t count;

    int fd = open(fileName, 0);
    if (fd == -1) {
        perror("open() ");
        free(buf);
        exit(EXIT_FAILURE);
    }

    do {
        count = read(fd, buf, 1);
        if (count == -1) {
            perror("read() ");
            free(buf);
            exit(EXIT_FAILURE);
        }
        printf("%s", buf);
    } while(count != 0);

    if (close(fd) == -1) {
        perror("close() ");
        free(buf);
        exit(EXIT_FAILURE);
    }

    free(buf);
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
    readFile(fileName);

    return 0;
}