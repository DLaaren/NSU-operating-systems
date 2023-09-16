#include <stdio.h>
#include <unistd.h> //unistd.h is the name of the header file that provides access to the POSIX operating system API

int main() {
    write(1, "Hello from write!\n", 19);
    return 0;
}