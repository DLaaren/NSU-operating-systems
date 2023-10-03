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
	return 42;
	//  pthread_exit(42);
}

void *mythread_hello(void *arg) {
	char str1[] = MESSAGE;
	char *str2 = malloc(sizeof(MESSAGE) * sizeof(char));
	strcpy(str2, MESSAGE);
	pthread_exit(str2);
}

int main() {
	pthread_t tid1, tid2;
	int err;

	printf("main [pid:%d parent-pid:%d thread-group-pid:%d]: Hello from main!\n", getpid(), getppid(), gettid());

	err = pthread_create(&tid1, NULL, mythread, NULL);
	if (err) {
	    printf("main: pthread_create() failed: %s\n", strerror(err));
		return -1;
	}

	// err = pthread_create(&tid2, NULL, mythread_hello, NULL);
	// if (err) {
	//     printf("main: pthread_create() failed: %s\n", strerror(err));
	// 	return -1;
	// }

    printf("main is waiting for threads\n");

	int retval;
    err = pthread_join(tid1, &retval);
    if (err) {
        printf("main: pthread_join() failed: %s\n", strerror(err));
        return -1;
    }

	// char *message;
	// err = pthread_join(tid2, &message);
    // if (err) {
    //     printf("main: pthread_join() failed: %s\n", strerror(err));
    //     return -1;
    // }
    
    printf("the thread has terminated with exit code %d; main continues working\n", retval);
	// printf("hello from another thread : %s\n", message);

    return 0;
}
