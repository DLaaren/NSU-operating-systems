#define _GNU_SOURCE
#include <unistd.h>
#include <sys/mman.h>
#include <linux/sched.h>
#include <sched.h>          
#include <sys/syscall.h>   
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PAGE_SIZE 4096

int writeNumbersToPipe(void* pipefd) {
    int pipe = *((int*)pipefd);
    for (unsigned int i = 0; i < PAGE_SIZE; i++) {
        int err = write(pipe, &i, sizeof(unsigned int));
        if (err == -1) {
            perror("write pipe");
        }
        // fprintf(stderr, "the function have written number %u\n", i);
        if (i == (PAGE_SIZE - 1)) {
            i = 0;
        }
    }
    return 0;
}

int readNumbersFromPipe(void* pipefd) {
    int pipe = *((int*)pipefd);

    unsigned int j = 1331;
    for (unsigned int i = 0; i < PAGE_SIZE; i++) {
        int err = read(pipe, &j, sizeof(unsigned int));
        if (err == -1) {
            perror("read pipe");
        } else if (j != i) {
            printf("Wrong written numbers at %u, but the result is %u!!\n", i, j);
        }
        if (i == (PAGE_SIZE - 1)) {
            i = 0;
        }
    }
}

int main() {
    void *memory_ptr1 = mmap(NULL, PAGE_SIZE, PROT_WRITE | PROT_READ, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    void *memory_ptr2 = mmap(NULL, PAGE_SIZE, PROT_WRITE | PROT_READ, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (memory_ptr1 == NULL || memory_ptr2 == NULL) {
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
    int pid1 = clone(writeNumbersToPipe, memory_ptr1 + PAGE_SIZE, CLONE_VM | CLONE_FILES | CLONE_FS | CLONE_NEWUTS | CLONE_NEWPID | SIGCHLD, &pipefd[1]);
    int pid2 = clone(readNumbersFromPipe, memory_ptr2 + PAGE_SIZE, CLONE_VM | CLONE_FILES | CLONE_FS | CLONE_NEWUTS | CLONE_NEWPID | SIGCHLD, &pipefd[0]);
    if (pid1 == -1 || pid2 == -1) {
        perror("clone()");
        return -1;
    }

    //pid_t waitpid(pid_t pid, int *status, int options);  
    int status1, status2;
    waitpid(pid1, &status1, 0);
    waitpid(pid2, &status2, 0);

    return 0;
}