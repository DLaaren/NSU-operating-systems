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

	pthread_attr_t attr;
	int err = pthread_attr_init(&attr);
	if (err) {
		printf("main: pthread_attr_init() failed: %s\n", strerror(err));
		return -1;
	}
	err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if (err) {
		printf("main: pthread_attr_setdetachstate() failed: %s\n", strerror(err));
		return -1;
	}	

	for (;;) {
		pthread_t tid;
		err = pthread_create(&tid, &attr, mythread, NULL);
		if (err) {
			printf("main: pthread_create() failed: %s\n", strerror(err));
			return -1;
		}
	}

	pthread_attr_destroy(&attr);
    pthread_exit(NULL);
}
