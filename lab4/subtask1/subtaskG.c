// how to create environment var
// export <varName>=<varValue>
// to print -- echo $<varName> 
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv, char** envp) {
    if (argc == 3 && setenv(argv[1], argv[2], 1) != 0) {
        perror("setenv()");
        exit(EXIT_FAILURE);
    }
    printf("value of environment variable: %s\n", getenv(argv[1]));
    if (setenv(argv[1], "CHANGED", 1) != 0) {
        perror("setenv()");
        exit(EXIT_FAILURE);
    }
    printf("changed value of environment variable: %s\n", getenv(argv[1]));
    return 0;
}