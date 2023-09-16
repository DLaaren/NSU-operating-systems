#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/user.h>

//https://habr.com/ru/post/111266/

//long ptrace(enum __ptrace_request request, pid_t pid, void *addr, void *data);
//request - действие, которое необходимо осуществить
//pid - индентификатор трассируемого процесса
//addr & data - зависят от request

//Начать трассировку можно двумя способами: 
//приаттачиться к уже запущенному процессу (PTRACE_ATTACH), 
//либо запустить его самому с помощью PTRACE_TRACEME.


int main() {
    pid_t pid = fork();
    //fork создает процесс-потомок
    //On success, the PID of the child process is returned in the
    //parent, and 0 is returned in the child.

    switch (pid) {
        case -1: //error
            perror("fork");
            exit(EXIT_FAILURE);
            break;

        case 0: //child's code
            //A traced mode has to be enabled
            ptrace(PTRACE_TRACEME, 0, 0, 0);
            //Семейство функций exec... загружает и запускает другие программы, известные как «дочерние» процессы. 
            //Если вызов функции exec... завершается успешно, «дочерний» процесс накладывается на «родительский» процесс
            execl("/bin/echo", "/bin/echo", "Hello, world!", NULL);
            break;

        default: //parent's code
            int status;
            wait(&status);
            ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_TRACESYSGOOD); //This makes it easy for the tracer
                                                                      // to distinguish normal traps from those caused by a system call.

            //не равно нулю, если дочерний процесс завершился
            while (!WIFEXITED(status)) {
                struct user_regs_struct registers;
                
                ptrace(PTRACE_SYSCALL, pid, 0, 0);
                wait(&status);
                
                if (WIFSTOPPED(status) && (WSTOPSIG(status) & 0x80) ) { //catch signal = interrupt
                    ptrace(PTRACE_GETREGS, pid, 0, &registers); //track registers
                    printf("SYSCALL %d at %08lx\n", registers.orig_rax, registers.rip);
                    
                    ptrace(PTRACE_SYSCALL, pid, 0, 0); //continue tracking
                    wait(&status);
                }
                
            }
    }
    return 0;
}