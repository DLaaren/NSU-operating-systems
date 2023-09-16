#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    printf("PID %d\n\n", getpid());

    int status;
    pid_t pid = fork();

    switch (pid) {
    case -1:
        perror("fork()");
        return -1;
    case 0: //child
        printf("child's PID: %d;   parent's PID: %d\n\n", getpid(), getppid());
        sleep(15);
        printf("child process terminated\n\n");
        _exit(5);
    default: //parent
        sleep(5);
        printf("parent process terminated\n\n");
    }
    
}