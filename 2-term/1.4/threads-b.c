#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

void *mythread(void *args) {
    // int err = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    // if (err) {
    //     printf("main: pthread_setcanceltype() failed: %s\n", strerror(err));
	// 	return NULL;
    // }
    for (int i = 0; ; i++) {
        pthread_testcancel();
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

    sleep(1);

    err = pthread_cancel(tid);
    if (err) {
        printf("main: pthread_cancel() failed: %s\n", strerror(err));
		return -1;
    }
    pthread_exit(NULL);
}