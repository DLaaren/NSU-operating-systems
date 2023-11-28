#define _GNU_SOURCE
#include <pthread.h>

#include "list.h"

List* list_init() {
    List *list;
    list = calloc(1, sizeof(List));
    pthread_mutex_init(&(list->mutex), NULL);
    return list;
}

void list_destroy(List* list) {
    int err;

    for (Node *curr_node = list->first; curr_node != NULL; ) {
        free(curr_node->value);
        Node *tmp = curr_node->next;
        err = pthread_mutex_destroy(&(curr_node->mutex));
        if (err == -1) {
            printf("list_destroy() : pthread_mutex_destroy() failed : %s\n", strerror(err));
            exit(1);
        }
        free(curr_node);
        curr_node = tmp;
    }
    free(list);
}

void list_add(List *list, char *value, size_t pos) {
    pos = (pos > list->size) ? list->size : pos;

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

Node* list_get(List *list, size_t pos) {

    pos = (pos > list->size) ? list->size : pos;

    Node *curr_node = list->first;
    for (size_t i = 0; i < pos; i++) {
        curr_node = curr_node->next;
    }

    return curr_node;
}

void list_erase(List *list, size_t pos) {
    pos = (pos > list->size) ? list->size : pos;

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

/* swap
     __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __                
    |           |              |                    |                       
    |   node    |  node->next  |  node->next->next  |
    |     1     |      2       |         3          |
    |__ __ __ __|__ __ __ __ __|__ __ __ __ __ __ __|

                        |
                        v
     __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ 
    |           |              |                    |
    |           |              |                    |
    |     1     |      3       |         2          |
    |__ __ __ __|__ __ __ __ __|__ __ __ __ __ __ __|

 */

int list_compare_values(List *list, Node *node) {
    if (node == NULL) {

        Node *first = list->first;
        Node *second = first->next;
        return strlen(first->value) - strlen(second->value);

    }
    else {

        Node *a = node->next;
        Node *b = node->next->next;
        return strlen(a->value) - strlen(b->value);

    }

}

void list_swap_elements(List *list, Node *node) {

    if (node == NULL) {

        Node *first = list->first;
        Node *second = first->next;

        first->next = second->next;
        second->next = first;

        list->first = second;

    }
    else {

        Node *a = node->next;
        Node *b = node->next->next;

        a->next = b->next;
        b->next = a;
        node->next = b;

    }
}

void list_print(List *list) {


    printf("\n\nStart printing list\n");

    for (Node *curr_node = list->first; curr_node != NULL; curr_node = curr_node->next) {
        printf("%s\n", curr_node->value);
    }

    printf("End printing list\n\n");

}