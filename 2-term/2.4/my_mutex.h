#ifndef MY_MUTEX_H
#define MY_MUTEX_H

#include <linux/futex.h>    
#include <sys/syscall.h>
#include <unistd.h>

typedef struct my_mutex_s {
    int lock;
} my_mutex_t;

void my_mutex_init(my_spinlock_t *mutex) {
    assert(mutex != NULL);
    mutex->lock = 1;
}

static int futex(int *uaddr, int futex_op, int val, const struct timespec *timeout, int *uaddr2, int val2) {
    return syscall(SYS_futex, uaddr, futex_op, val, timeout, uaddr2, val2);
}

void my_mutex_lock(my_spinlock_t *mutex) {
    int err;
    assert(mutex != NULL);
    while (1) {
        const int one = 1;
        if (atomic_compare_exchange_strong(&mutex->lock, &one, 0))
            break;
        err = futex(&mutex->lock, FUTEX_WAIT, 0, NULL, NULL, 0);
        if (err == -1 && errno != EAGAIN) {
            printf("futex wait failed:: %s\n", strerror(errno));
            abort();
        }
    }
}

void my_mutex_unlock(my_spinlock_t *mutex) {
    int err;
    assert(mutex != NULL);
    const int zero = 0;
    if (atomic_compare_exchange_strong(&mutex->lock, &zero, 1)) {
        err = futex(&mutex->lock, FUTEX_WAKE, 1, NULL, NULL, 0);
        if (err == -1) {
            printf("futex wake failed:: %s\n", strerror(errno));
            abort();
        }
    }
}

#endif // MY_MUTEX_H