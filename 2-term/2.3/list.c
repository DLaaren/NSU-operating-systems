#define _GNU_SOURCE
#include <pthread.h>

#include "list.h"

List* list_init() {
    List *list;
    list = calloc(1, sizeof(List));
    return list;
}

void list_destroy(List* list) {
    int err;
    Node *curr_node = list->first;
    for (size_t i = 0; i < list->size || curr_node != NULL; i++) {
        Node *next_node = curr_node->next;
        err = pthread_mutex_destroy(&(curr_node->mutex));
        if (err == -1) {
            printf("list_destroy() : pthread_mutex_destroy() failed : %s\n", strerror(err));
            exit(1);
        }
        free(curr_node);
        curr_node = next_node;
    }
    free(list);
}

void list_add(List *list, char *value, size_t pos) {
    if (pos > list->size) {
        pos = list->size;
    }
    int err;
    Node *curr_node = list->first;
    for (size_t i = 0; i < pos; i++) {
        curr_node = curr_node->next;
    }
    Node *new_node = calloc(1, sizeof(Node));
    err = pthread_mutex_init(&(new_node->mutex), NULL);
    if (err == -1) {
        printf("list_add() : pthread_mutex_init() failed : %s\n", strerror(err));
        free(new_node);
        exit(2);
    }
    if (pos == 0) {
        new_node->next = curr_node;
        list->first = new_node;
    }
    else {
        new_node->next = curr_node->next;
        curr_node->next = new_node;
    }
    new_node->value = value;
    list->size++;
}

char* list_get(List *list, char *value, size_t pos) {
    if (pos > list->size) {
        pos = list->size;
    }
    Node *curr_node = list->first;
    for (size_t i = 0; i < pos; i++) {
        curr_node = curr_node->next;
    }
    return curr_node->value;
}

void list_erase(List *list, char *value, size_t pos) {
    if (pos > list->size) {
        pos = list->size;
    }
    Node *curr_node = list->first;
    for (size_t i = 0; i < pos; i++) {
        curr_node = curr_node->next;
    }
    Node *node_to_delete;
    if (pos == 0) {
        node_to_delete = list->first;
        list->first = node_to_delete->next;
    }
    else {
        node_to_delete = curr_node->next;
        curr_node->next = node_to_delete->next;
    }
    free(node_to_delete);
}
