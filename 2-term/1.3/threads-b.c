#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

typedef struct arg {
    int num;
    char *string;
} arg;

void *mythread2(void *my_arg) {
	printf("got struct with fields : \"%d\" and \"%s\"\n", ((arg*)my_arg)->num, ((arg*)my_arg)->string);
	return NULL;
}

void *mythread(void *my_arg) {
	printf("got struct with fields : \"%d\" and \"%s\"\n", ((arg*)my_arg)->num, ((arg*)my_arg)->string);

	pthread_t tid;
	int err;
    pthread_attr_t attr;
	err = pthread_attr_init(&attr);
	if (err) {
		printf("main: pthread_attr_init() failed: %s\n", strerror(err));
		return NULL;
	}
	err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if (err) {
		printf("main: pthread_attr_setdetachstate() failed: %s\n", strerror(err));
		return NULL;
	}	

	err = pthread_create(&tid, &attr, mythread2, my_arg);
	if (err) {
	    printf("main: pthread_create() failed: %s\n", strerror(err));
		return NULL;
	}

	pthread_attr_destroy(&attr);

	free(my_arg);
    return NULL;
}

int main() {
    arg *my_arg = malloc(sizeof(arg));
    my_arg->num = 10;
    char string[] = "my string";
    my_arg->string = string;

	printf("struct fields : \"%d\" and \"%s\"\n", my_arg->num, my_arg->string);

	pthread_t tid;
	int err;
    pthread_attr_t attr;
	err = pthread_attr_init(&attr);
	if (err) {
		printf("main: pthread_attr_init() failed: %s\n", strerror(err));
		return -1;
	}
	err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if (err) {
		printf("main: pthread_attr_setdetachstate() failed: %s\n", strerror(err));
		return -1;
	}	

	err = pthread_create(&tid, &attr, mythread, my_arg);
	if (err) {
	    printf("main: pthread_create() failed: %s\n", strerror(err));
		return -1;
	}

	pthread_attr_destroy(&attr);
	pthread_exit(NULL);
}
