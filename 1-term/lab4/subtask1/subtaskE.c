#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void foo() {
    char* buffer = (char*)malloc(100);
    char* phrase = "hello world";

    memcpy(buffer, phrase, 12);

    printf("%s\n", buffer);
    free(buffer);
    printf("%s\n", buffer);

    char* anotherBuffer = (char*)malloc(100);
    char* anotherPhrase = "hello world again";
    memcpy(anotherBuffer, anotherPhrase, 18);
    printf("%s\n", anotherBuffer);

    char* p = anotherBuffer + 50;
    free(p);
    printf("%s\n", anotherBuffer);

    free(anotherBuffer);
}

int main() {
    foo();
    return 0;
}