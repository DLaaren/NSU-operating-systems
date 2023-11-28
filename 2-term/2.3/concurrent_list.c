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
int list_elements_equals;

pthread_mutex_t print_mutex;

void* monitor(void *arg) {
    List *list = (List*)arg;

    // while (1) {
        printf("looking_for_increasing_string_length_itertions :: %d\n \
                looking_for_decreasing_string_length_itertions :: %d\n \
                looking_for_equal_string_length_itertions :: %d\n \
                list_elements_swaps :: %d\n \
                list_elements_equals :: %d\n\n", 
                looking_for_increasing_string_length_itertions, looking_for_decreasing_string_length_itertions, 
                looking_for_equal_string_length_itertions, list_elements_swaps, list_elements_equals);
        list_print(list);
        // sleep(1);
    // }

    return NULL;
}

void* looking_for_increasing_string_length(void *arg) {
    List *list = (List*) arg;
    while (1) {
        long int curr_pos = 0;
        Node *prev_node = NULL;
        Node *curr_node = list_get(list, curr_pos);
        Node *next_node = list_get(list, curr_pos + 1);
        curr_pos++;

        printf("before swap : next :: %s\npos :: %ld\n\n", next_node->value, curr_pos);

        if (list_compare_values(list, prev_node) < 0) {
            list_swap_elements(list, prev_node);
            list_elements_swaps++;
        }
        monitor(list);

        for(; curr_pos + 1 < list->size; curr_pos++) {
            prev_node = list_get(list, curr_pos - 1);
            curr_node = list_get(list, curr_pos);
            next_node = list_get(list, curr_pos + 1);

            printf("before swap :: prev :: %s next :: %s\npos :: %ld\n\n", prev_node->value, next_node->value, curr_pos);

            if (list_compare_values(list, prev_node) < 0) {
                list_swap_elements(list, prev_node);
                list_elements_swaps++;
            }

            printf("after swap\n");

            monitor(list);
            sleep(1);
        }
        looking_for_increasing_string_length_itertions++;
    }
    return NULL;
}

void* looking_for_decreasing_string_length(void *arg) {
    List *list = (List*) arg;
    while (1) {
        Node *prev_node = NULL;
        Node *curr_node = list_get(list, 0);
        Node *next_node = list_get(list, 1);

        if (list_compare_values(list, prev_node) > 0) {
            list_swap_elements(list, prev_node);
            list_elements_swaps++;
        }

        for(long int curr_pos = 2; curr_pos < list->size; curr_pos++) {
            prev_node = curr_node;
            curr_node = next_node;
            next_node = list_get(list, curr_pos);

            if (list_compare_values(list, prev_node) > 0) {
                list_swap_elements(list, prev_node);
                list_elements_swaps++;
            }
        }
        looking_for_decreasing_string_length_itertions++;
    }
    return NULL;
}

void* looking_for_equal_string_length(void *arg) {
    List *list = (List*) arg;
    while (1) {
        Node *prev_node = NULL;
        Node *curr_node = list_get(list, 0);
        Node *next_node = list_get(list, 1);

        if (list_compare_values(list, prev_node) == 0) {
            list_elements_equals++;
        }

        for(long int curr_pos = 2; curr_pos < list->size; curr_pos++) {
            prev_node = curr_node;
            curr_node = next_node;
            next_node = list_get(list, curr_pos);

            if (list_compare_values(list, prev_node) == 0) {
                list_elements_equals++;
            }
        }
        looking_for_equal_string_length_itertions++;
    }
    return NULL;
}

int main() {
    srand(time(NULL));
    const char ALLOWED[] = "abcdefghijklmnopqrstuvwxyz123456789";
    pthread_t tid;
    List *list;
    int list_size = 5;
    int err0, err1, err2, err3;

    printf("main [pid : %d; ppid : %d; tid : %d]\n", getpid(), getppid(), gettid());

    list = list_init();

    for (long int i = 0; i < list_size; i++) {
        int rand_size = (rand() % 100);
        int value_size = rand_size ? rand_size : 1;
        char *value = malloc(value_size * sizeof(char));
        for (long int k = 0; k < value_size; k++) {
            char c = ALLOWED[rand() % sizeof(ALLOWED)];
            value[k] = c;
        }
        list_add(list, value, 0);
    }

    list_print(list);

    sleep(1);

    err0 = 0; //pthread_create(&tid, NULL, monitor, (void*)list);

    err1 = pthread_create(&tid, NULL, looking_for_increasing_string_length, (void*)list);

    err2 = 0; //pthread_create(&tid, NULL, looking_for_decreasing_string_length, (void*)list);

    err3 = 0;//pthread_create(&tid, NULL, looking_for_equal_string_length, (void*)list); 
	
    if ((err0 | err1 | err2 | err3) != 0) {
		printf(RED"main: pthread_create() failed: %s\n"NOCOLOR, strerror(err1));
		return -1;
	}

    pthread_exit(NULL);
}