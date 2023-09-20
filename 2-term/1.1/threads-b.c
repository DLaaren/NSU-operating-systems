#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#define THREADS_NUM 5

void *mythread(void *arg) {
	printf("mythread [pid:%d parent-pid:%d thread-group-pid:%d]: Hello from mythread!\n", getpid(), getppid(), gettid());
	return NULL;
}

int main() {
	pthread_t tid[THREADS_NUM];
	int err;

	printf("main [pid:%d parent-pid:%d thread-group-pid:%d]: Hello from main!\n", getpid(), getppid(), gettid());

    for (int i = 0; i < THREADS_NUM; i++) {
	    err = pthread_create(&tid[i], NULL, mythread, NULL);
        if (err) {
            printf("main: pthread_create() failed: %s\n", strerror(err));
            return -1;
        }
    }

	pthread_exit(NULL);
}

