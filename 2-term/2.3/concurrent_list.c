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

#include "list.h"

#define RED "\033[41m"
#define NOCOLOR "\033[0m"

int looking_for_increasing_string_length_itertions;
int looking_for_decreasing_string_length_itertions;
int looking_for_equal_string_length_itertions;
int list_elements_swaps;
int list_elements_equal;

/*
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

void list_swap_elements(List *list, Node *node) {
    if (node == NULL) {
        Node *first = list->first;
        Node *second = first->next;
        list->first = second;

        first->next = second->next;
        second->next = first;
    }
    else {
        Node *a = node->next;
        Node *b = node->next->next;
        node->next = b;
        node->next->next = a;
    }
}

void* looking_for_increasing_string_length(void *arg) {
    List *list = (List*) arg;
    while (1) {
        Node *prev_node = NULL;
        Node *curr_node = list->first;
        size_t list_size = list->size;
        Node *next_node = curr_node->next;

        for(size_t i = 0; i < list_size; i++) {
            if (strcmp(curr_node->value, next_node->value) < 0) {
                list_swap_elements(list, prev_node);
                list_elements_swaps++;
            }
            prev_node = curr_node;
            curr_node = next_node;
            next_node = next_node->next;
        }
        looking_for_increasing_string_length_itertions++;
    }
    return NULL;
}

void* looking_for_decreasing_string_length(void *arg) {
    List *list = (List*) arg;
    while (1) {
        Node *prev_node = NULL;
        Node *curr_node = list->first;
        size_t list_size = list->size;
        Node *next_node = curr_node->next;

        for(size_t i = 0; i < list_size; i++) {
            if (strcmp(curr_node->value, next_node->value) > 0) {
                list_swap_elements(list, prev_node);
                list_elements_swaps++;
            }
            prev_node = curr_node;
            curr_node = next_node;
            next_node = next_node->next;
        }
        looking_for_decreasing_string_length_itertions++;
    }
    return NULL;
}

void* looking_for_equal_string_length(void *arg) {
    List *list = (List*) arg;
    while (1) {
        Node *curr_node = list->first;
        Node *next_node = curr_node->next;
        size_t list_size = list->size;

        for(size_t i = 0; i < list_size; i++) {
            if (strcmp(curr_node->value, next_node->value) == 0) {
                list_elements_equal++;
            }
            curr_node = next_node;
            next_node = next_node->next;
        }
        looking_for_equal_string_length_itertions++;
    }
    return NULL;
}

int main() {
    srand(time(NULL));
    pthread_t tid;
    List *list;
    int list_size = 1000000;
    int err1, err2, err3;

    printf("main [pid : %d; ppid : %d; tid : %d]\n", getpid(), getppid(), gettid());

    list = list_init();

    for (size_t i = 0; i < list_size; i++) {
        char *value = malloc((rand() % 100) * sizeof(char));
        list_add(list, value, 0);
    }

    err1 = pthread_create(&tid, NULL, looking_for_increasing_string_length, (void*)list);

    err2 = pthread_create(&tid, NULL, looking_for_decreasing_string_length, (void*)list);

    err3 = pthread_create(&tid, NULL, looking_for_equal_string_length, (void*)list); 
	
    if ((err1 | err2 | err3) != 0) {
		printf(RED"main: pthread_create() failed: %s\n"NOCOLOR, strerror(err1));
		return -1;
	}

    pthread_exit(NULL);
}