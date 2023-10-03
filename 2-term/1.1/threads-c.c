#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

int global_var = 50;

void *mythread(void *v) {
	printf("mythread [pid:%d\nparent-pid:%d\nthread-pid\n(gettid):%d\n(pthread_self):%ld]\n",
            getpid(), getppid(), gettid(), pthread_self());

    printf("var address from thread:%p\n\n", &v);
	return NULL;
}

int main() {
    int local_var = 10;
    int const local_const_var = 20;
    int static local_static_var = 30;

	pthread_t tid;
	int err;

	err = pthread_create(&tid, NULL, mythread, &local_var);
    printf("\npthread_create returned %ld\n", tid);
	if (err) {
	    printf("main: pthread_create() failed: %s\n", strerror(err));
		return -1;
	}

    printf("from main\nlocal_var:%p\nlocal_const_var:%p\nlocal_static_var:%p\nglobal_var:%p\n\n",
            &local_var, &local_const_var, &local_static_var, &global_var);

	pthread_exit(NULL);
}

