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
unsigned long long list_elements_swaps1;
unsigned long long list_elements_swaps2;
unsigned long long list_elements_equals;

void* monitor(void *arg) {
    List *list = (List*)arg;

    while (1) {
        printf("looking_for_increasing_string_length_itertions :: %llu\n \
                looking_for_decreasing_string_length_itertions :: %llu\n \
                looking_for_equal_string_length_itertions :: %llu\n \
                list_elements_swaps1 :: %llu\n \
                list_elements_swaps2 :: %llu\n \
                list_elements_equals :: %llu\n\n", 
                looking_for_increasing_string_length_itertions, looking_for_decreasing_string_length_itertions, 
                looking_for_equal_string_length_itertions, list_elements_swaps1, list_elements_swaps2, list_elements_equals);
        list_print(list);
        sleep(1);
    }

    return NULL;
}

void* looking_for_increasing_string_length(void *arg) {
    printf("INCREMENT COMPORATOR STARTED\n");

    List *list = (List*) arg;
    while (1) {
        long int curr_pos = 0;
        Node *prev_node = NULL;
        curr_pos++;

        if (list_compare_values(list, prev_node) < 0) {
            list_swap_elements(list, prev_node);
            list_elements_swaps1++;
        }

        for(; curr_pos + 1 < list->size; curr_pos++) {
            prev_node = list_get(list, curr_pos - 1);

            if (list_compare_values(list, prev_node) < 0) {
                list_swap_elements(list, prev_node);
                list_elements_swaps1++;
            }
        }

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
        long int curr_pos = 0;
        Node *prev_node = NULL;
        curr_pos++;

        if (list_compare_values(list, prev_node) > 0) {
            list_swap_elements(list, prev_node);
            list_elements_swaps2++;
        }

        for(; curr_pos + 1 < list->size; curr_pos++) {
            prev_node = list_get(list, curr_pos - 1);

            if (list_compare_values(list, prev_node) > 0) {
                list_swap_elements(list, prev_node);
                list_elements_swaps2++;
            }
        }
        
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
        long int curr_pos = 0;
        Node *prev_node = NULL;
        curr_pos++;

        if (list_compare_values(list, prev_node) == 0) {
            list_elements_equals++;
        }

        for(; curr_pos + 1 < list->size; curr_pos++) {
            prev_node = list_get(list, curr_pos - 1);

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
    int list_size = LIST_SIZE;
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

    sleep(2);

    err0 = pthread_create(&tid, NULL, monitor, (void*)list);

    err1 = 0; //pthread_create(&tid, NULL, looking_for_increasing_string_length, (void*)list);

    err2 = pthread_create(&tid, NULL, looking_for_decreasing_string_length, (void*)list);

    err3 = 0;//pthread_create(&tid, NULL, looking_for_equal_string_length, (void*)list); 
	
    if ((err0 | err1 | err2 | err3) != 0) {
		printf(RED"main: pthread_create() failed: %s\n"NOCOLOR, strerror(err1));
		return -1;
	}

    pthread_exit(NULL);
}