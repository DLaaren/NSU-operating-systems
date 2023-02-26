#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

//https://habr.com/ru/post/111266/

//long ptrace(enum __ptrace_request request, pid_t pid, void *addr, void *data);
//request - действие, которое необходимо осуществить
//pid - индентификатор трассируемого процесса
//addr & data - зависят от request

//Начать трассировку можно двумя способами: 
//приаттачиться к уже запущенному процессу (PTRACE_ATTACH), 
//либо запустить его самому с помощью PTRACE_TRACEME.


int main() {
    int rv;
    pid_t pid;
    //fork создает процесс-потомок
    //On success, the PID of the child process is returned in the
    //parent, and 0 is returned in the child.
    switch (pid = fork()) {
        case -1: //error
            perror("fork");
            exit(1);
        case 0: //child's code
            printf(" CHILD: Это процесс-потомок!\n");
            printf(" CHILD: Мой PID -- %d\n", getpid());
            printf(" CHILD: PID моего родителя -- %d\n", getppid());
            printf(" CHILD: Введите мой код возврата (как можно меньше): ");
            scanf("%d", &rv);
            printf(" CHILD: Выход!\n");
            exit(rv);
        default: //parent's code
            printf("PARENT: Это процесс-родитель!\n");
            printf("PARENT: Мой PID -- %d\n", getpid());
            printf("PARENT: PID моего потомка %d\n", pid);
            printf("PARENT: Я жду, пока потомок не вызовет exit()...\n");
            wait(&rv);
            printf("PARENT: Код возврата потомка: %d\n", WEXITSTATUS(rv));
            printf("PARENT: Выход!\n");
    }
    return 0;
}