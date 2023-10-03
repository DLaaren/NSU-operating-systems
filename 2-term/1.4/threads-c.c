#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

static int execute = 0;

void cleanup_handler(void *args) {
    printf("Cleaning up\n");
    if (args != NULL) {
        free(args);
    }
}

void *newthread(void *args) {
    char *hello_string = malloc(25 * sizeof(char));

    pthread_cleanup_push(cleanup_handler, hello_string);

    printf("string pointer from newthread : %p\n", hello_string);
    strcpy(hello_string, "Hello from my thread!");
    for (;;) {
        printf("%s\n", (char*)hello_string);
    }

    pthread_cleanup_pop(execute);
    pthread_exit(hello_string);
}

void *mythread(void *args) {
    int err;
    // err = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    // if (err) {
    //     printf("main: pthread_setcanceltype() failed: %s\n", strerror(err));
	// 	return NULL;
    // }

    pthread_t newtid;
    err = pthread_create(&newtid, NULL, newthread, NULL);
    if (err) {
        printf("mythread: pthread_create() failed: %s\n", strerror(err));
    }

    sleep(1);

    err = pthread_cancel(newtid);
    if (err) {
        printf("mythread: pthread_cancel() failed: %s\n", strerror(err));
    }

    void *retval;
    err = pthread_join(newtid, &retval);
    printf("string pointer from mythread : address %p\n", retval);
    printf("string %s\n", (char *)retval);

    pthread_exit(NULL);
}

int main() {
    pthread_t tid;
    int err;

    err = pthread_create(&tid, NULL, mythread, NULL);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
		return -1;
    }

    pthread_exit(NULL);
}