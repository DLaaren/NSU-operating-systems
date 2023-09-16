#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

#define MESSAGE "Hello from thread!\n"

void *mythread(void *arg) {
	printf("mythread [pid:%d parent-pid:%d thread-group-pid:%d]: Hello from mythread!\n", getpid(), getppid(), gettid());
	pthread_exit(NULL);
}

int main() {

	printf("main [pid:%d parent-pid:%d thread-group-pid:%d]: Hello from main!\n\n", getpid(), getppid(), gettid());

	for (;;) {
		pthread_t tid;
		int err = pthread_create(&tid, NULL, mythread, NULL);
		if (err) {
			printf("main: pthread_create() failed: %s\n", strerror(err));
			return -1;
		}
	}

    pthread_exit(NULL);
}
