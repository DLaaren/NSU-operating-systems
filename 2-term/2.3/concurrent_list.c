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

unsigned long long looking_for_increasing_string_length_itertions;
unsigned long long looking_for_decreasing_string_length_itertions;
unsigned long long looking_for_equal_string_length_itertions;
unsigned long long list_elements_increasing;
unsigned long long list_elements_decreasing;
unsigned long long list_elements_equals;
unsigned long long list_elements_swaps;

void* monitor(void *arg) {
    while (1) {
        printf("looking_for_increasing_string_length_itertions :: %llu\n \
                looking_for_decreasing_string_length_itertions :: %llu\n \
                looking_for_equal_string_length_itertions :: %llu\n \
                list_elements_increasing :: %llu\n \
                list_elements_decreasing :: %llu\n \
                list_elements_equals :: %llu\n \
                list_elements_swaps :: %llu\n\n", 
                looking_for_increasing_string_length_itertions, looking_for_decreasing_string_length_itertions, 
                looking_for_equal_string_length_itertions, list_elements_increasing, list_elements_decreasing, list_elements_equals, list_elements_swaps);
        // list_print(list);
        sleep(1);
    }

    return NULL;
}

void* looking_for_increasing_string_length(void *arg) {
    printf("INCREMENT COMPORATOR STARTED\n");

    int mode = NONE_MODE;
    #ifdef RWLOCK
    mode = READ_MODE;
    #endif

    List *list = (List*) arg;
    while (1) {
        lock(list->first, mode);
        lock(list->first->next, mode);

        Node *prev_node = NULL;
        Node *curr_node = list->first;
        Node *next_node = list->first->next;

        while (next_node != NULL) {
            if (list_compare_values(list, prev_node) < 0) {
                list_elements_increasing++;
            }
            
            if (prev_node != NULL) 
                unlock(prev_node);

            if (next_node->next != NULL)
                lock(next_node->next, mode);

            prev_node = curr_node;
            curr_node = next_node;
            next_node = next_node->next;

        }

        unlock(prev_node);
        unlock(curr_node);

        looking_for_increasing_string_length_itertions++;
    }
    return NULL;
}

void* looking_for_decreasing_string_length(void *arg) {
    printf("DECREMENT COMPORATOR STARTED\n");

    int mode = NONE_MODE;
    #ifdef RWLOCK
    mode = READ_MODE;
    #endif

    List *list = (List*) arg;
    while (1) {
        lock(list->first, mode);
        lock(list->first->next, mode);

        Node *prev_node = NULL;
        Node *curr_node = list->first;
        Node *next_node = list->first->next;

        while (next_node != NULL) {
            if (list_compare_values(list, prev_node) > 0) {
                list_elements_decreasing++;
            }

            if (prev_node != NULL) 
                unlock(prev_node);

            if (next_node->next != NULL)
                lock(next_node->next, mode);

            prev_node = curr_node;
            curr_node = next_node;
            next_node = next_node->next;
        }
        
        unlock(prev_node);
        unlock(curr_node);

        looking_for_decreasing_string_length_itertions++;
    }
    return NULL;
}

void* looking_for_equal_string_length(void *arg) {
    printf("EQUAL COMPORATOR STARTED\n");

    int mode = NONE_MODE;
    #ifdef RWLOCK
    mode = READ_MODE;
    #endif

    List *list = (List*) arg;
    while (1) {
        lock(list->first, mode);
        lock(list->first->next, mode);

        Node *prev_node = NULL;
        Node *curr_node = list->first;
        Node *next_node = list->first->next;

        while (next_node != NULL) {
            if (list_compare_values(list, prev_node) == 0) {
                list_elements_equals++;
            }

            if (prev_node != NULL) 
                unlock(prev_node);

            if (next_node->next != NULL)
                lock(next_node->next, mode);

            prev_node = curr_node;
            curr_node = next_node;
            next_node = next_node->next;
        }

        unlock(prev_node);
        unlock(curr_node);

        looking_for_equal_string_length_itertions++;
    }
    return NULL;
}

void* randomly_swapper_thread(void *arg) {
    printf("SWAPPER THREAD STARTED\n");

    int mode = NONE_MODE;
    #ifdef RWLOCK
    mode = WRITE_MODE;
    #endif

    List *list = (List*) arg;
    while (1) {
        lock(list->first, mode);
        lock(list->first->next, mode);

        Node *prev_node = list->first;
        Node *curr_node = list->first->next;
        Node *next_node;

        while (curr_node->next != NULL) {
            lock(curr_node->next, mode);
            next_node = curr_node->next;
        
            if (rand() % 4 == 0) {
                list_swap_elements(list, prev_node);
                list_elements_swaps++;

                unlock(prev_node);
                prev_node = next_node;
            }
            else {
                unlock(prev_node);
                prev_node = curr_node;
                curr_node = next_node;
            }
        }

        unlock(prev_node);
        unlock(curr_node);
    }
}

int main() {
    // srand(time(NULL));
    const char ALLOWED[] = "abcdefghijklmnopqrstuvwxyz123456789";
    pthread_t tid;
    List *list;
    int err0, err1, err2, err3, err4, err5, err6;

    printf("main [pid : %d; ppid : %d; tid : %d]\n", getpid(), getppid(), gettid());

    list = list_init();

    for (long int i = 0; i < LIST_SIZE; i++) {
        int rand_size = (rand() % 100);
        int value_size = rand_size ? rand_size : 1;
        char *value = malloc(value_size * sizeof(char));
        for (long int k = 0; k < value_size; k++) {
            char c = ALLOWED[rand() % sizeof(ALLOWED)];
            value[k] = c;
        }
        list_add(list, value, 0);
    }

    // list_print(list);

    sleep(2);

    err0 = pthread_create(&tid, NULL, monitor, (void*)list);

    err1 = pthread_create(&tid, NULL, looking_for_increasing_string_length, (void*)list);

    err2 = pthread_create(&tid, NULL, looking_for_decreasing_string_length, (void*)list);

    err3 = pthread_create(&tid, NULL, looking_for_equal_string_length, (void*)list);

    err4 = pthread_create(&tid, NULL, randomly_swapper_thread, (void*)list);
    err5 = pthread_create(&tid, NULL, randomly_swapper_thread, (void*)list);
    err6 = pthread_create(&tid, NULL, randomly_swapper_thread, (void*)list);
	
    if ((err0 | err1 | err2 | err3 | err4 | err5 | err6) != 0) {
		printf(RED"main: pthread_create() failed: %s\n"NOCOLOR, strerror(err1));
		return -1;
	}

    pthread_exit(NULL);
}