#ifndef __FITOS_QUEUE_H__
#define __FITOS_QUEUE_H__

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct _QueueNode {       // нода очереди
	int val;
	struct _QueueNode *next;
} qnode_t;

typedef struct _Queue {           // очередь
	qnode_t *first;
	qnode_t *last;

	pthread_t qmonitor_tid;

	int count;
	int max_count;

	// queue statistics
	long add_attempts; // сколько попыток добавить/получить значение было сделано
	long get_attempts;
	long add_count;    // сколько попыток было успешно
	long get_count;
} queue_t;

queue_t* queue_init(int max_count);	// создаёт пустую очередь
void queue_destroy(queue_t *q);
int queue_add(queue_t *q, int val);
int queue_get(queue_t *q, int *val);
void queue_print_stats(queue_t *q);

#endif		// __FITOS_QUEUE_H__
