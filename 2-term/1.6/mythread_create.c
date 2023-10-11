// Разработать собственную функцию для создания ядерных потоков - аналог
// pthread_create():
// int mythread_create(mythread_t *thread, void *(start_routine), void *arg);
// Функция должна возвращать успех-неуспех.

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
#include <unistd.h>
#include <fcntl.h>

#define CLONE_MYTHREAD_FLAGS (CLONE_VM | CLONE_FILES | CLONE_THREAD | CLONE_SIGHAND | SIGCHLD)

#define PAGE_SIZE 4096
// #define STACK_SIZE getrlimit(RLIMIT_STACK, &limit)
#define STACK_SIZE 3*PAGE_SIZE

typedef void *(*start_routine_t)(void*);

typedef struct mythread_s {
    int mythread_id;
    start_routine_t start_routine;
    void *arg;
    void *retval;

    volatile int isFinished;
    volatile int isJoined;

    // sigmask
    // *stack_base
    // stack size
    // recieved signal
} mythread_s;

typedef mythread_s* mythread_t;

// обертка / промежуточная функция
int mythread_startup(void *arg) {
    mythread_t tid = (mythread_t)arg;
    mythread_s *mythread = tid;
    void *retval;

    printf("thread_startup() : starting a thread function for thread %d\n", mythread->mythread_id);
    
    retval = mythread->start_routine(mythread->arg);

    mythread->retval = retval;
    mythread->isFinished = 1;

    printf("thread_startup() : waiting for a mythread_join() for thread %d\n", mythread->mythread_id);

    while (!mythread->isJoined) {
        sleep(1);
    }

    printf("thread_startup() : the thread function finished for thread %d\n", mythread->mythread_id);
    return 0;
}

void *create_stack(off_t stack_size, int mytid) {
    char stack_file[128];
    int stack_fd;
    void *stack;

    printf("create_stack() : creating stack for thread %d\n", mytid);
    snprintf(stack_file, sizeof(stack_file), "stack-%d", mytid);

    stack_fd = open(stack_file, O_RDWR | O_CREAT, 0660);
    ftruncate(stack_fd, 0);
    ftruncate(stack_fd, stack_size);

    stack = mmap(NULL, stack_size, PROT_NONE, MAP_SHARED, stack_fd, 0);
    close(stack_fd);

    printf("create_stack() : created for thread %d\n", mytid);

    return stack;
}

int mythread_create(mythread_t *mytid, void *(*start_routine)(void *), void *arg) {
    static int thread_id = 0;
    mythread_s *mythread; // allocate it on the bottom of the stack -- so when you delete thread stack you also delete the structure of this thread
    int child_pid;
    void *mythread_stack;

    thread_id++;

    printf("mythread_create() : creating thread %d\n", thread_id);

    mythread_stack = create_stack(STACK_SIZE, thread_id);

    mprotect(mythread_stack + PAGE_SIZE, STACK_SIZE - PAGE_SIZE, PROT_READ | PROT_WRITE);

    memset(mythread_stack + PAGE_SIZE, 0x7f, STACK_SIZE - PAGE_SIZE);

    mythread = (mythread_s *)(mythread_stack + STACK_SIZE - sizeof(mythread_s));
    mythread->mythread_id = thread_id;
    mythread->start_routine = start_routine;
    mythread->arg = arg;
    mythread->retval = NULL;
    mythread->isJoined = 0;
    mythread->isFinished = 0;

    mythread_stack = (void *)mythread;

    printf("mythread_stack : %p;\n", mythread_stack);

    child_pid = clone(mythread_startup, mythread_stack, CLONE_MYTHREAD_FLAGS, mythread);
    if (child_pid == -1) {
        printf("clone() failed: %s\n", strerror(errno));
        exit(-1);
    }
    
    *mytid = mythread;
    return 0;  
}

int mythread_join(mythread_t mytid, void **retval) {
    mythread_s *mythread = mytid;

    printf("mythread_join() : waiting for thread %d to finish\n", mythread->mythread_id);

    while (!mythread->isFinished) {
        sleep(1);
    }

    printf("mythread_join() : thread %d finished\n", mythread->mythread_id);
    
    *retval = mythread->retval;
    mythread->isJoined = 1;
    return 0;
}

void *mythread_func(void *arg) {
    char *str = (char *)arg;
    for (int i = 0; i < 5; i++) {
        printf("msg: %s\n", str);
        sleep(1);
    }

    return "I've finished";
}

int main() {
    mythread_t mytid;
    void *retval;

    printf("[main pid:%d ppid:%d tid:%d]\n", getpid(), getppid(), gettid());

    mythread_create(&mytid, mythread_func, "wow I'm working!");

    mythread_join(mytid, &retval);

    printf("[main pid:%d ppid:%d tid:%d]\nmyhread returned: %s\n", getpid(), getppid(), gettid(), (char *)retval);

    return 0;
}