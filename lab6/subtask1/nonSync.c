#define _GNU_SOURCE
#include <unistd.h>
#include <sys/mman.h>
#include <linux/sched.h>
#include <sched.h>          
#include <sys/syscall.h>   
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

#define PAGE_SIZE 4096

int writeNumbers(void* memory_ptr) {
    for (unsigned int i = 0; i < PAGE_SIZE; i++) {
        *((unsigned int*)memory_ptr + i) = i;
        if (i == PAGE_SIZE - 1) {
            i = 0;
        }
    }
}

int readNumbers(void* memory_ptr) {
    for (unsigned int i = 0; i < PAGE_SIZE; i++) {
        if (*((unsigned int*)memory_ptr + i) != i) {
            printf("Wrong written numbers at %u, but the result is %u!!\n", i, *((unsigned int*)memory_ptr + i));
        }
        if (i == PAGE_SIZE - 1) {
            i = 0;
        }
    }
}

int main() {
    int output_fd = open("/home/dlaaren/Desktop/lab6/subtask1/output.txt", O_RDWR);
    if (output_fd == -1) {
        perror("open()");
        return -1;
    }
    void *memory_ptr = mmap(NULL, PAGE_SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, output_fd, 0);
    if (memory_ptr == NULL) {
        perror("mmap()");
        return -1;
    }
    int pid1 = clone(writeNumbers, memory_ptr + PAGE_SIZE, CLONE_VM | CLONE_FILES | CLONE_FS, memory_ptr);
    int pid2 = clone(readNumbers, memory_ptr + PAGE_SIZE, CLONE_VM | CLONE_FILES | CLONE_FS, memory_ptr);
    if (pid1 == -1 || pid2 == -1) {
        perror("clone()");
        return -1;
    }
    return 0;
}