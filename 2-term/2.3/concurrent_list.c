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

void looking_for_increasing_string_length(void *arg) {

}

void looking_for_decreasing_string_length(void *arg) {

}

void looking_for_equal_string_length(void *arg) {

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
		printf(RED"main: pthread_create() failed: %s\n"NOCOLOR, strerror(err));
		return -1;
	}

    pthread_exit(NULL);
}