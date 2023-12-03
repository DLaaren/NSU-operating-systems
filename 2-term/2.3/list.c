#define _GNU_SOURCE
#include <pthread.h>

#include "list.h"

#define RED "\033[41m"
#define NOCOLOR "\033[0m"

void lock(Node *node, int mode) {
    #ifdef SPINLOCK
    if (pthread_spin_lock(&(node->spinlock)) != 0) {
        printf(RED"Error while locking spinlock :: %s\n"NOCOLOR, strerror(errno));
        exit(1);
    }
    #endif

    #ifdef MUTEX
    if (pthread_mutex_lock(&(node->mutex)) != 0) {
        printf(RED"Error while locking mutex :: %s\n"NOCOLOR, strerror(errno));
        exit(1);
    }
    #endif

    #ifdef RWLOCK
    if (mode == READ_MODE) {
        if (pthread_rwlock_rdlock(&(node->rwlock)) != 0) {
            printf(RED"Error while locking rwlock in reader mode :: %s\n"NOCOLOR, strerror(errno));
            exit(1);
        }
    }
    else if (mode == WRITE_MODE) {
        if (pthread_rwlock_wrlock(&(node->rwlock)) != 0) {
            printf(RED"Error while locking rwlock in writer mode :: %s\n"NOCOLOR, strerror(errno));
            exit(1);
        }
    }
    else {
        printf(RED"Invalid mode in argument\n"NOCOLOR);
        exit(1);
    }
    #endif
}

void unlock(Node *node) {
    #ifdef SPINLOCK
    if (pthread_spin_unlock(&(node->spinlock)) != 0) {
        printf(RED"Error while unlocking spinlock :: %s\n"NOCOLOR, strerror(errno));
        exit(1);
    }
    #endif

    #ifdef MUTEX
    if (pthread_mutex_lock(&(node->mutex)) != 0) {
        printf(RED"Error while locking mutex :: %s\n"NOCOLOR, strerror(errno));
        exit(1);
    }
    #endif

    #ifdef RWLOCK
    if (pthread_rwlock_unlock(&(node->rwlock)) != 0) {
        printf(RED"Error while unlocking rwlock :: %s\n"NOCOLOR, strerror(errno));
        exit(1);
    }
    #endif
}

List* list_init() {
    List *list;
    list = calloc(1, sizeof(List));
    return list;
}

void list_destroy(List* list) {
    while (list->size < LIST_SIZE) {
        list_erase(list, list->size);
    }
    free(list);
}

void list_add(List *list, char *value, long int pos) {
    Node *new_node;
    Node *curr_node;

    assert(list != NULL);
    assert(list->size < LIST_SIZE);

    pos = (pos > list->size) ? list->size : pos;

    curr_node = list->first;
    for (long int i = 0; i < pos; i++) {
        curr_node = curr_node->next;
    }
    
    new_node = calloc(1, sizeof(Node));

    #ifdef SPINLOCK
    if (pthread_spin_init(&(new_node->spinlock), 0) != 0) {
        printf(RED"Cannot create spinlock for new node :: %s\n"NOCOLOR, strerror(errno));
        exit(2);
    }
    #endif

    #ifdef MUTEX
    if (pthread_mutex_init(&(new_node->mutex), 0) != 0) {
        printf(RED"Cannot create mutex for new node :: %s\n"NOCOLOR, strerror(errno));
        exit(2);
    }
    #endif

    #ifdef RWLOCK
    if (pthread_rwlock_init(&(new_node->rwlock), 0) != 0) {
        printf(RED"Cannot create rwlock for new node :: %s\n"NOCOLOR, strerror(errno));
        exit(2);
    }
    #endif

    if (pos == 0) {
        new_node->next = curr_node;
        list->first = new_node;
    }
    else {
        new_node->next = curr_node->next;
        curr_node->next = new_node;
        if (pos == list->size) {
            list->last = new_node;
        }
    }
    new_node->value = value;
    list->size++;
}

Node* list_get(List *list, long int pos) {
    Node *curr_node;

    assert(list != NULL);
    assert(list->size > 0);

    pos = (pos >= list->size) ? list->size : pos;

    curr_node = list->first;
    for (long int i = 0; i < pos; i++) {
        curr_node = curr_node->next;
    }

    return curr_node;
}

void list_erase(List *list, long int pos) {
    Node *node_to_delete;
    Node *curr_node;

    assert(list != NULL);
    assert(list->size > 0);

    pos = (pos > list->size) ? list->size : pos;

    curr_node = list->first;
    for (long int i = 0; i < pos; i++) {
        curr_node = curr_node->next;
    }

    if (pos == 0) {
        node_to_delete = list->first;
        list->first = node_to_delete->next;
    }
    else {
        node_to_delete = curr_node->next;
        curr_node->next = node_to_delete->next;
        if (pos == list->size) {
            list->last = curr_node;
        }
    }

    #ifdef SPINLOCK
    pthread_spin_destroy(&(node_to_delete->spinlock));
    #endif

    #ifdef MUTEX
    pthread_mutex_destroy(&(node_to_delete->mutex));
    #endif

    #ifdef RWLOCK
    pthread_rwlock_destroy(&(node_to_delete->rwlock));
    #endif

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

long int list_compare_values(List *list, Node *node) {
    long int size_diff;
    if (node == NULL) {
        Node *first = list->first;
        Node *second = first->next;

        size_diff = strlen(first->value) - strlen(second->value);
    }
    else {
        Node *a = node->next;
        Node *b = node->next->next;

        size_diff = strlen(a->value) - strlen(b->value);
    }
    return size_diff;

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

        if (b == list->last) {
            list->last = a;
        }

        a->next = b->next;
        b->next = a;
        node->next = b;
    }
}

void list_print(List *list) {
    printf("\n\nStart printing list\n");

    Node *curr_node = list->first;

    while (curr_node != NULL) {
        printf("%s\n", curr_node->value);
        curr_node = curr_node->next;
    }

    printf("End printing list\n\n");
}