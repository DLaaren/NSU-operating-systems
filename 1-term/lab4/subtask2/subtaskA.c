#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> 

int main(int argc, char** argv) {
    printf("PID %d\n", getpid());
    sleep(1);
    if (execl(argv[0], "./a.out", (char*)NULL) != 0) {
        perror("execl()");
        exit(EXIT_FAILURE);
    }
    printf("Hello world\n");
    return 0;
}