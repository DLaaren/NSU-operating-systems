#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

void *mythread1(void *args) {
    sigset_t mask;
    sigfillset(&mask);
    //pthread_sigmask(SIG_BLOCK, &mask, NULL);
    printf("Hello from mythread 1!\n");

    sleep(2);
    for (int i = 0; i < 10000; i++) {
        sleep(1);
        printf("thread 1 is still working : %d\n", i);
    }

    pthread_exit(NULL);
}

void sigint_handler() {
    printf("mythread 2 got SIGINT\n");
    pthread_exit(NULL);
}

void *mythread2(void *args) {
    struct sigaction act;
    act.sa_sigaction = &sigint_handler;

    int err = sigaction(SIGINT, &act, NULL);
    if (err) {
        printf("mythread 1: sigaction() failed: %s\n", strerror(err));
    }

    printf("Hello from mythread 2!\n");

    sleep(2);
    for (int i = 0; i < 7; i++) {
        sleep(1);
        printf("thread 2 is still working : %d\n", i);
    }

    pthread_exit(NULL);
}

void *mythread3(void *args) {
    int signum;
    sigset_t mask;
    sigaddset(&mask, SIGQUIT);
    sigwait(&mask, &signum);
    printf("Hello from mythread 3!\n");
    pthread_exit(NULL);
}

int main() {
    pthread_t tid1, tid2, tid3;
    int err;

    err = pthread_create(&tid1, NULL, mythread1, NULL);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
		return -1;
    }

    err = pthread_create(&tid2, NULL, mythread2, NULL);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
		return -1;
    }

    err = pthread_create(&tid3, NULL, mythread3, NULL);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
		return -1;
    }

    sleep(1);
    pthread_kill(tid3, SIGINT);
    // pthread_kill(tid3, SIGINT); //call handler for SIGINT

    sleep (1);
    pthread_kill(tid2, SIGINT);

    sleep (1);
    pthread_kill(tid1, SIGINT); //can not kill pthread 1 sending any of the signals

    pthread_exit(NULL);
}