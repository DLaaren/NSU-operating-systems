#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

void *mythread(void *args) {
    for (;;) {
        printf("Hello from mythread!\n");
    }
}

int main() {
    pthread_t tid;
    int err;

    err = pthread_create(&tid, NULL, mythread, NULL);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
		return -1;
    }

    err = pthread_cancel(tid);
    if (err) {
        printf("main: pthread_cancel() failed: %s\n", strerror(err));
		return -1;
    }
    pthread_exit(NULL);
}