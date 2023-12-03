#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

#include "my_spinlock.h"
#include "my_mutex.h"

#define RED "\033[41m"
#define NOCOLOR "\033[0m"

#define SPINLOCK
// #define MUTEX

#ifdef SPINLOCK
my_spinlock_t lock;
#endif

#ifdef MUTEX
my_mutex_t lock;
#endif

volatile int val =  0;

void* inc(void *arg) {
	for (int i = 0; i < 10000; i++) {

		#ifdef SPINLOCK
		my_spinlock_lock(&lock);
		#endif

		#ifdef MUTEX
		my_mutex_lock(&lock);
		#endif

		val++;

		#ifdef SPINLOCK
		my_spinlock_unlock(&lock);
		#endif

		#ifdef MUTEX
		my_mutex_unlock(&lock);
		#endif
	}

	return NULL;
}

int main() {
	pthread_t tid1, tid2;
	int err;
	int retval;

	#ifdef SPINLOCK
	my_spinlock_init(&lock);
	#endif
	#ifdef MUTEX
	my_mutex_init(&lock);
	#endif

	err = pthread_create(&tid1, NULL, inc, NULL); 
	if (err) {
		printf("main: pthread_create() failed: %s\n", strerror(err));
		return -1;
	}

	err = pthread_create(&tid2, NULL, inc, NULL);  
	if (err) {
		printf("main: pthread_create() failed: %s\n", strerror(err));
		return -1;
	}

	err = pthread_join(tid1, (void *)(&retval));
	if (err) {
		printf("main: pthread_join() failed: %s\n", strerror(err));
		return -1;
	}
	err = pthread_join(tid2, (void *)(&retval));
	if (err) {
		printf("main: pthread_join() failed: %s\n", strerror(err));
		return -1;
	}

	printf("val = %d\n", val);

	pthread_exit(NULL);

	return 0;
}
