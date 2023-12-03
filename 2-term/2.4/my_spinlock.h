#ifndef MY_SPINLOCK_H
#define MY_SPINLOCK_H

#include <stdatomic.h>
#include <assert.h>

typedef struct my_spinlock_s {
    int lock;
} my_spinlock_t;

void my_spinlock_init(my_spinlock_t *spinlock) {
    assert(spinlock != NULL);
    spinlock->lock = 1;
}

void my_spinlock_lock(my_spinlock_t *spinlock) {
    assert(spinlock != NULL);
    while (1) {
        const int one = 1;
        if (atomic_compare_exchange_strong(&spinlock->lock, &one, 0))
            break;
    }
}

void my_spinlock_unlock(my_spinlock_t *spinlock) {
    assert(spinlock != NULL);
    const int zero = 0;
    atomic_compare_exchange_strong(&spinlock->lock, &zero, 1);
}

#endif // MY_SPINLOCK_H