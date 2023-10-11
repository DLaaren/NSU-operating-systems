#define _GNU_SOURCE
#include <pthread.h>
#include <assert.h>

#include "queue.h"

void *qmonitor(void *arg) {
	queue_t *q = (queue_t *)arg;

	printf("qmonitor: [pid : %d; pprid : %d; tid : %d]\n", getpid(), getppid(), gettid());

	while (1) {
		queue_print_stats(q);
		sleep(1);
	}

	return NULL;
}

queue_t* queue_init(int max_count) { // аргумент = макс кол-во нод в спсике
	int err;

	queue_t *q = malloc(sizeof(queue_t));   // выделяем память под структуру очереди
	if (!q) {
		printf("Cannot allocate memory for a queue\n");
		abort();
	}

	q->first = NULL;
	q->last = NULL;
	q->max_count = max_count;
	q->count = 0;              // кол-во нод в настоящий момент

	q->add_attempts = q->get_attempts = 0;
	q->add_count = q->get_count = 0;

	err = pthread_create(&q->qmonitor_tid, NULL, qmonitor, q);  // создаём поток, его tid сохраняем в очереди, запускаем qmonitor с параметром q = созданная очередь
	if (err) {
		printf("queue_init: pthread_create() failed: %s\n", strerror(err));
		abort();
	}

	return q; // возвращаем указатель на очередь
}

void queue_destroy(queue_t *q) {
	qnode_t *curr_node = q->first;
	while (curr_node != NULL) {
		qnode_t *tmp = curr_node;
		curr_node = curr_node->next;
		free(tmp);
	}
	free(q);
}

int queue_add(queue_t *q, int val) {
	q->add_attempts++;					// + 1 попытка

	assert(q->count <= q->max_count);   // проверка можем ли мы добавить элемент в очередь

	if (q->count == q->max_count)
		return 0;

	qnode_t *new = malloc(sizeof(qnode_t));   // выделяем память под ноду
	if (!new) {
		printf("Cannot allocate memory for new node\n");
		abort();
	}

	new->val = val;
	new->next = NULL;

	if (!q->first)
		q->first = q->last = new;
	else {
		q->last->next = new;
		q->last = q->last->next;
	}

	q->count++;
	q->add_count++;    // + 1 удачная попытка

	return 1;
}

int queue_get(queue_t *q, int *val) {
	q->get_attempts++; 		// + 1 попытка

	assert(q->count >= 0);  // проверка можно ли что-то достать

	if (q->count == 0)
		return 0;

	qnode_t *tmp = q->first; // сохраняем указатель на первую ноду

	*val = tmp->val; // достаём значение
	q->first = q->first->next; // теперь первой нодой в очереди становится следущая нода

	free(tmp); // а самую первую, значение которой мы достали, удаляем
	q->count--; // уменьшаем длину очереди
	q->get_count++; // + 1 удачная попытка

	return 1;
}

void queue_print_stats(queue_t *q) { // просто печатает кол-ва попыток и кол-во удачных попыток
	printf("\nqueue stats: current size %d;\nattempts: (add_attempts: %ld; get_attempts: %ld; add_attempts - get_attempts: %ld);\ncounts (add_count: %ld; get_count: %ld; add_count - get_count: %ld)\n",
		q->count,
		q->add_attempts, q->get_attempts, q->add_attempts - q->get_attempts,
		q->add_count, q->get_count, q->add_count -q->get_count);
}

