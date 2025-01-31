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

void * buf_append_begin(struct buf * ctx);
void buf_append_end(struct buf * ctx);
void buf_remove(struct buf * ctx, void * item);

void buf_iter_begin(struct buf * ctx);
void buf_iter_end(struct buf * ctx);
void * buf_get(struct buf * ctx, size_t idx);

#endif