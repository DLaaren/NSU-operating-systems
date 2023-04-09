#define _XOPEN_SOURCE_EXTENDED 1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <signal.h>
#include <setjmp.h>

jmp_buf ebuf;
int sigsegv_handler_called = 0;


void allocatorStack(int i) {
    printf("%d\n", i);
    i++;
    char buffer[1000000] = {0}; // stack overflow on 7-8th iteration 
    sleep(1);
    allocatorStack(i);
}

void allocatorHeap(int i) {
    printf("%d\n", i);
    i++;
    while (i < 100) {
        char *buffer = (char*)malloc(1000000);
        sleep(2);
        allocatorHeap(i);
        free(buffer);
    }
}

void signalHandler() {
    write(STDOUT_FILENO, "You can not access the data!\n", 30);
    sigsegv_handler_called = 1;
    longjmp(ebuf, 0); //look in signal-safety
}

int main() {
    printf("PID %d\n", getpid());
    sleep(10);
    int i = 0;
    //allocatorStack(i);
    //allocatorHeap(i);
    char* ptr = mmap(NULL, 4096 * 10, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap()");
        exit(EXIT_FAILURE);
    }
    printf("mmaped\n");

    if (sigset(SIGSEGV, signalHandler) != 0) {
        perror("sigset()");
    }

    int jmp_location = setjmp(ebuf);

    if (sigsegv_handler_called == 0) {
        printf("read from mmapped pages: %d\n", *ptr);
        *ptr = 15;
        printf("write to mmapped pages: %d\n", *ptr);
    }

    sleep(10);

    if (munmap(ptr + 4096 * 3, 4096 * 3) != 0) {
        perror("sigaction()");
    } else {
        printf("munmapped 4-6th pages\n");
    }

    sleep(10);
   
    if (munmap(ptr, 4096 * 10) != 0) {
        perror("sigaction()");
        exit(EXIT_FAILURE);
    }
    return 0;
}   