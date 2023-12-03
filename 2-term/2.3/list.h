#ifndef __LIST_H__
#define __LIST_H__

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>

#define LIST_SIZE 10

#define READ_MODE 1
#define WRITE_MODE 0
#define NONE_MODE 2

#define SPINLOCK
// #define MUTEX
// #define RWLOCK

typedef struct Node_s {
    char *value;
    struct Node_s *next;

    #ifdef SPINLOCK
    pthread_spinlock_t spinlock;
    #endif

    #ifdef MUTEX
    pthread_mutex_t mutex;
    #endif

    #ifdef RWLOCK
    pthread_rwlock_t rwlock;
    #endif
} Node;

typedef struct List_s {
    Node *first;
    Node *last;
    long int size;
} List;

List* list_init();
void list_destroy(List *list);
void list_add(List *list, char *value, long int pos);
Node* list_get(List *list, long int pos);
void list_erase(List *list, long int pos);
long int list_compare_values(List *list, Node *node);
void list_swap_elements(List *list, Node *node);
void list_print(List *list);

#endif		// __LIST_H__