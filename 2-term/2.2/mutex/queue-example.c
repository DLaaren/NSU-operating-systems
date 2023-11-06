#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sched.h>

#include "queue.h"

int main() {
	queue_t *q;

	printf("main: [pid : %d; ppid : %d; tid : %d]\n", getpid(), getppid(), gettid());

	q = queue_init(1000); // создаём очередь с макс числом нод = 1000

	for (int i = 0; i < 10; i++) {
		int ok = queue_add(q, i);   // добавляем значение в нашу очередь

		printf("ok %d: add value %d\n", ok, i);  // печатаем удалось ли добавить и какое значение

		queue_print_stats(q);   // печатаем параметры (попытки)
	}

	for (int i = 0; i < 12; i++) {
		int val = -1;
		int ok = queue_get(q, &val);  // достаём значение из очереди

		printf("ok: %d: get value %d\n", ok, val);  // печатаем удалось ли достать и какое значение

		queue_print_stats(q);    // печатаем параметры (попытки)
	}

	queue_destroy(q);   // удаляем очередь

	return 0;
}

