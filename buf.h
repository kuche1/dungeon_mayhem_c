#ifndef BUF_H
#define BUF_H

#include <pthread.h>
#include <stddef.h>

typedef int (* buf_sameas_function) (void * a, void * b, size_t item_size);
// `a` and `b` are the items being compared
// `item_size` is only used internally by the generic memory comparator, in case this function has not been overwritten; it shouldn't be of use to you, but you CAN use it if you feel like it
// if the two items differ the return code is 0

struct buf
{
    char * mem;
    size_t item_size;
    buf_sameas_function sameas_fnc;

    size_t len;
    size_t size;

    pthread_mutex_t lock;
};

void buf_init(struct buf * ctx, size_t item_size, buf_sameas_function sameas_fnc);
// `sameas_fnc` can be NULL, in that case the whole item is going to be compared bit-by-bit

void buf_deinit(struct buf * ctx);

void buf_lock(struct buf * ctx);
void buf_unlock(struct buf * ctx);

void * buf_append(struct buf * ctx); // needs to be used with in a locked buffer in multithreaded environment

void buf_remove(struct buf * ctx, void * item);
// needs to be used with in a locked buffer in multithreaded environment
// internally `sameas_fnc` to compare the items

void buf_clear(struct buf * ctx, size_t items_to_keep); // needs to be used with in a locked buffer in multithreaded environment

void * buf_get(struct buf * ctx, size_t idx); // result needs to be used with in a locked buffer in multithreaded environment
void * buf_get_all(struct buf * ctx); // result needs to be used with in a locked buffer in multithreaded environment

#endif