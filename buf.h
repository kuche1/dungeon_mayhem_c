#ifndef BUF_H
#define BUF_H

#include <pthread.h>
#include <stddef.h>

struct buf
{
    char * mem;
    size_t item_size;

    size_t len;
    size_t size;

    pthread_mutex_t lock;
};

void buf_init(struct buf * ctx, size_t item_size);
void buf_deinit(struct buf * ctx);

void buf_lock(struct buf * ctx);
void buf_unlock(struct buf * ctx);

void * buf_append(struct buf * ctx); // needs to be used with in a locked buffer in multithreaded environment

void buf_remove(struct buf * ctx, void * item); // needs to be used with in a locked buffer in multithreaded environment
void buf_clean(struct buf * ctx); // needs to be used with in a locked buffer in multithreaded environment

void * buf_get(struct buf * ctx, size_t idx); // result needs to be used with in a locked buffer in multithreaded environment
void * buf_get_all(struct buf * ctx); // result needs to be used with in a locked buffer in multithreaded environment

#endif