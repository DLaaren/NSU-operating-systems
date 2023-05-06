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

int writeNumbersToPipe(void* pipefd) {
    int pipe = *((int*)pipefd);
    for (unsigned int i = 0; i < PAGE_SIZE; i++) {
        write(pipe, &i, sizeof(unsigned int));
        if (i == (PAGE_SIZE - 1)) {
            i = 0;
        }
    }
}

int readNumbersFromPipe(void* pipefd) {
    int pipe = *((int*)pipefd);
    unsigned int j = 101;
    for (unsigned int i = 0; i < PAGE_SIZE; i++) {
        read(pipe, &j, sizeof(unsigned int));
        if (j != i) {
            printf("Wrong written numbers at %u, but the result is %u!!\n", i, j);
        }
        if (i == (PAGE_SIZE - 1)) {
            i = 0;
        }
    }
}

int main() {
    int output_fd = open("/home/dlaaren/Desktop/lab6/subtask2/output.txt", O_RDWR);
    if (output_fd == -1) {
        perror("open()");
        return -1;
    }
    void *memory_ptr = mmap(NULL, PAGE_SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, output_fd, 0);
    if (memory_ptr == NULL) {
        perror("mmap()");
        return -1;
    }

    //creating pipe
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe()");
        return -1;
    }
    //pipefd[0] refers to the read end of the pipe. pipefd[1] refers to the write end of the pipe.
    int pid1 = clone(writeNumbersToPipe, memory_ptr + PAGE_SIZE, CLONE_VM | CLONE_FILES | CLONE_FS, &pipefd[1]);
    sleep(0.5);
    int pid2 = clone(readNumbersFromPipe, memory_ptr + PAGE_SIZE, CLONE_VM | CLONE_FILES | CLONE_FS, &pipefd[0]);
    if (pid1 == -1 || pid2 == -1) {
        perror("clone()");
        return -1;
    }
    return 0;
}