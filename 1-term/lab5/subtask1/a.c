#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int globalVar = 5;

int main() {
    int localVar = 10;
    printf("globalVar: %d;  its address: %p\nlocalVar: %d;  its addres: %p\n", globalVar, &globalVar, localVar, &localVar);
    printf("PID %d\n\n", getpid());

    int status;
    pid_t pid = fork();

    switch (pid) {
    case -1:
        perror("fork()");
        return -1;
    case 0: //child
        printf("child's PID: %d;   parent's PID: %d\n\n", getpid(), getppid());
        printf("CHILD::\nglobalVar: %d;  its address: %p\nlocalVar: %d;  its addres: %p\n", globalVar, &globalVar, localVar, &localVar);
        globalVar = 50;
        localVar = 100;
        printf("CHILD(changed)::\nglobalVar: %d;  its address: %p\nlocalVar: %d;  its addres: %p\n\n", globalVar, &globalVar, localVar, &localVar);
        printf("child process terminated\n\n");
        _exit(5);
    default: //parent
        sleep(15);
        printf("PARENT::\nglobalVar: %d;  its address: %p\nlocalVar: %d;  its addres: %p\n", globalVar, &globalVar, localVar, &localVar);
        //The wait() system call suspends execution of the calling process until one of its children terminates.
        if (wait(&status) == -1) {
            perror("wait()");
        }
        if (WIFEXITED(status)) {
            printf("child process terminated normally: %d\n", WEXITSTATUS(status));
        }
        else if (WIFSIGNALED(status)) {
            printf("child process was ternimated by signal: %d\n", WTERMSIG(status));
        }
        else if (WIFSTOPPED(status)) {
            printf("child process was stopped by signal: %d\n", WSTOPSIG(status));
        }
        printf("Status: %d\n", status);
        return 0;
    }
    
}