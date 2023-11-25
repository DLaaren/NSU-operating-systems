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
#include <semaphore.h>

#include "queue.h"

#define RED "\033[41m"
#define NOCOLOR "\033[0m"

int size = 1000000;

sem_t sem_read;
sem_t sem_write;
sem_t sem_lock;

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

		sem_wait(&sem_read);
		sem_wait(&sem_lock);

		int val = -1;
		int ok = queue_get(q, &val); // достаём значение с очереди
		if (expected != val)
			printf(RED"ERROR: get value is %d but expected  %d" NOCOLOR "\n", val, expected);

		expected = val + 1;

		sem_post(&sem_lock);
		sem_post(&sem_write);
	}

	return NULL;
}

void *writer(void *arg) {
	int i = 0;
	queue_t *q = (queue_t *)arg;
	printf("writer [pid : %d; ppid : %d; tid : %d]\n", getpid(), getppid(), gettid());

	set_cpu(2);

	while (1) {

		sem_wait(&sem_write);
		sem_wait(&sem_lock);

		// usleep(1);
		int ok = queue_add(q, i); // добавляем значение в очередь
		i++;

		sem_post(&sem_lock);
		sem_post(&sem_read);

	}

	return NULL;
}


int main() {
	pthread_t tid;
	queue_t *q;
	int err;

	printf("main [pid : %d; ppid : %d; tid : %d]\n", getpid(), getppid(), gettid());

	q = queue_init(size);   // создаём очередь
	
	if (sem_init(&sem_read, 0, 0) || sem_init(&sem_write, 0, size) || sem_init(&sem_lock, 0, 1)) {
		printf("ERROR: sem_init() in main()\n");
		return -1;
	}

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

	if (sem_destroy(&sem_read) || sem_destroy(&sem_write) || sem_destroy(&sem_lock)) {
		printf("ERROR: sem_destroy() in main()\n");
		return -1;
	}

	// TODO: join threads

	pthread_exit(NULL);

	return 0;
}
