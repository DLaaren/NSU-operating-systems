#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#include <pthread.h>
#include <sched.h>
#include <linux/futex.h>
#include <sys/syscall.h>
#include <stdint.h>

#include "queue.h"

#define RED "\033[41m"
#define NOCOLOR "\033[0m"

void set_cpu(int n) {
	int err;
	cpu_set_t cpuset;
	pthread_t tid = pthread_self();

	CPU_ZERO(&cpuset); 		// зануляем сет
	CPU_SET(n, &cpuset);	// сет ядер = n

	err = pthread_setaffinity_np(tid, sizeof(cpu_set_t), &cpuset); // определяет на каком ядре будет выполняться !системный! поток
	if (err) {
		printf("set_cpu: pthread_setaffinity failed for cpu %d\n", n);
		return;
	}

	printf("set_cpu: set cpu %d\n", n);
}

void *reader(void *arg) {
	int expected = 0;
	queue_t *q = (queue_t *)arg;
	printf("reader [pid : %d; ppid : %d; tid : %d]\n", getpid(), getppid(), gettid());

	set_cpu(1);

	while (1) {
		int val = -1;
		int ok = queue_get(q, &val); // достаём значение с очереди
		if (!ok) {
			continue;
		}
		if (expected != val)
			printf(RED"ERROR: get value is %d but expected  %d" NOCOLOR "\n", val, expected);

		expected = val + 1;
	}

	return NULL;
}

void *writer(void *arg) {
	int i = 0;
	queue_t *q = (queue_t *)arg;
	printf("writer [pid : %d; ppid : %d; tid : %d]\n", getpid(), getppid(), gettid());

	set_cpu(2);

	while (1) {
		// usleep(1);
		int ok = queue_add(q, i); // добавляем значение в очередь
		if (!ok) {
			continue;
		}
		i++;
	}

	return NULL;
}

int main() {
	pthread_t tid;
	queue_t *q;
	int err;

	printf("main [pid : %d; ppid : %d; tid : %d]\n", getpid(), getppid(), gettid());

	q = queue_init(1000000);   // создаём очередь
	

	err = pthread_create(&tid, NULL, reader, q);   // создаём поток на чтение, передаём нашу очередь
	if (err) {
		printf("main: pthread_create() failed: %s\n", strerror(err));
		return -1;
	}

	sched_yield();  // планировщик потоков 
					// sched_yield() causes the calling thread to relinquish the CPU.
       				// The thread is moved to the end of the queue for its static
       				// priority and a new thread gets to run.

	err = pthread_create(&tid, NULL, writer, q);   // создаём поток на запись, передаём нашу очередь
	if (err) {
		printf("main: pthread_create() failed: %s\n", strerror(err));
		return -1;
	}

	// TODO: join threads

	pthread_exit(NULL);

	return 0;
}
