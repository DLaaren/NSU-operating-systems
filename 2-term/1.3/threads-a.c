#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct arg {
    int num;
    char *string;
} arg;

void *mythread(void *my_arg) {
	printf("got struct with fields : \"%d\" and \"%s\"\n", ((arg*)my_arg)->num, ((arg*)my_arg)->string);
	return NULL;
}

int main() {
	pthread_t tid;
	int err;
    arg my_arg;
    my_arg.num = 10;
    char string[] = "my string";
    my_arg.string = string;

	printf("struct fields : \"%d\" and \"%s\"\n", my_arg.num, my_arg.string);

	err = pthread_create(&tid, NULL, mythread, &my_arg);
	if (err) {
	    printf("main: pthread_create() failed: %s\n", strerror(err));
		return -1;
	}

	pthread_exit(NULL);
}
