#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <linux/futex.h>
#include <sys/syscall.h> 
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <ucontext.h>

#define RED "\033[41m"
#define NOCOLOR "\033[0m"

#define CLONE_MYTHREAD_FLAGS (CLONE_VM | CLONE_FILES | CLONE_THREAD | CLONE_SIGHAND | SIGCHLD)

#define MAX_THREADS 4
#define STACK_SIZE 8192

typedef void *(*start_routine_t)(void*);

typedef struct uthread {
    int tid;
    start_routine_t start_routine;
    void *arg;
    void *retval;
    ucontext_t ucntx;
} uthread_t;

uthread_t *uthreads[MAX_THREADS];
int uthread_count;
int uthread_curr;
ucontext_t *main_cntx;

int uthread_finished_count;

void *create_stack(off_t stack_size, int mytid) {
    char stack_file[128];
    int stack_fd;
    void *stack;

    printf("create_stack() : creating stack for thread %d\n", mytid);
    snprintf(stack_file, sizeof(stack_file), "stack-%d", mytid);

    stack_fd = open(stack_file, O_RDWR | O_CREAT, 0660);
    ftruncate(stack_fd, 0);
    ftruncate(stack_fd, stack_size);

    stack = mmap(NULL, stack_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_STACK, stack_fd, 0);
    close(stack_fd);

    printf("create_stack() : created for thread %d\n", mytid);

    return stack;
}

void uthread_shedule(void) {
    ucontext_t *curr_cntx, *next_cntx;    
    curr_cntx = &(uthreads[uthread_curr]->ucntx);
    uthread_curr = (uthread_curr + 1) % uthread_count;
    next_cntx = &(uthreads[uthread_curr]->ucntx);

    int err = swapcontext(curr_cntx, next_cntx);
    if (err == -1) {
        printf("uthread_shedule() : swapcontext() failed: %s\n", strerror(err));
        exit(1);
    }
    sleep(1);
    printf("context swaped from thread %d to thread %d;\n", uthreads[uthread_curr]->tid, uthreads[(uthread_curr + 1) % uthread_count]->tid);
}

void uthread_create(uthread_t **uthread, void *(*start_routine)(void *), void *arg) {
    static int uthread_id = 1;
    uthread_t *uthread_new;

    void *stack = create_stack(STACK_SIZE, uthread_id);
    uthread_new = (uthread_t*)(stack + STACK_SIZE - sizeof(uthread_t));

    int err = getcontext(&uthread_new->ucntx);
    if (err == -1) {
        printf("uthread_create() : getcontext() failed: %s\n", strerror(err));
        exit(2);
    }

    uthread_new->ucntx.uc_stack.ss_sp = stack;
    uthread_new->ucntx.uc_stack.ss_size = STACK_SIZE - sizeof(uthread_t);
    uthread_new->ucntx.uc_link = main_cntx;

    uthread_new->start_routine = start_routine;
    uthread_new->arg = arg;
    uthread_new->tid = uthread_id;
    uthread_id++;

    uthreads[uthread_count++] = uthread_new;
    *uthread = uthread_new;

    printf("thread %d created\n", uthread_new->tid);

    makecontext(&uthread_new->ucntx, uthread_new->start_routine, 0);
}

void thread1(void *arg) {
    printf("[thread 1 pid:%d ppid:%d tid:%d]\n", getpid(), getppid(), gettid());
    for (int i = 0; i < 3; i++) {
        uthread_shedule();
    }
    printf("[thread 1 finished]\n");
    uthread_finished_count++;
}

void thread2(void *arg) {
    printf("[thread 2 pid:%d ppid:%d tid:%d]\n", getpid(), getppid(), gettid());
    for (int i = 0; i < 3; i++) {
        uthread_shedule();
    }
    printf("[thread 2 finished]\n");
    uthread_finished_count++;
}

void thread3(void *arg) {
    printf("[thread 3 pid:%d ppid:%d tid:%d]\n", getpid(), getppid(), gettid());
    for (int i = 0; i < 3; i++) {
        uthread_shedule();
    }
    printf("[thread 3 finished]\n");
    uthread_finished_count++;
}

int main() {
    int p;
    uthread_t *my_uthreads[3];
    uthread_t main_thread;

    main_thread.ucntx.uc_stack.ss_sp = &p;
    main_thread.ucntx.uc_stack.ss_size = STACK_SIZE;
    main_thread.ucntx.uc_link = NULL;
    main_thread.tid = 0;

    main_cntx = &main_thread.ucntx;
    
    uthreads[0] = &main_thread;
    uthread_count++;

    printf("[main pid:%d ppid:%d tid:%d]\n", getpid(), getppid(), gettid());

    uthread_create(&my_uthreads[0], thread1, NULL);
    uthread_create(&my_uthreads[1], thread2, NULL);
    uthread_create(&my_uthreads[2], thread3, NULL);

    while(uthread_finished_count != 3) {
        uthread_shedule();
    }

    printf("[main finished]\n");

    return 0;
}