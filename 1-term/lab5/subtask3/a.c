#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/sched.h>
#include <sched.h>
#include <sys/wait.h>
#include <stdlib.h>

#define STACK_SIZE 1024
// fallocate -l 4096 output.txt

void allocateStringOnStack(int depth) {
    depth--;
    char string[13] = "Hello World!";
    while (depth > 0) {
        allocateStringOnStack(depth);
    }
}

int entryPoint() {
    allocateStringOnStack(10);
    return 0;
}

int main() {
    int output_fd = open("/home/dlaaren/Desktop/lab5/subtask3/output.txt", O_RDWR);
    if (output_fd == -1) {
        perror("open()");
        return -1;
    }
    void* child_stack = mmap(NULL, STACK_SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, output_fd, 0);
    if (child_stack == NULL) {
        perror("mmap()");
        return -1;
    }

    pid_t clone_pid = clone(entryPoint, child_stack + STACK_SIZE,  CLONE_VM, NULL);
    if (clone_pid == -1) {
        perror("clone()");
        return -1;
    }
    return 0;
}