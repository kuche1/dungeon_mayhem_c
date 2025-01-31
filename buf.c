
// TODO
// maybe we should just let the end user do the locking and unlocking himself for all fncs

#include "buf.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define BUF_INITIAL_SIZE 1 // this must be at least 1

static void lock(pthread_mutex_t * lock)
{
    if(pthread_mutex_lock(lock)){
        fprintf(stderr, "ERROR: could not lock buf mutex\n");
        exit(1);
    }
}

static void unlock(pthread_mutex_t * lock)
{
    if(pthread_mutex_unlock(lock)){
        fprintf(stderr, "ERROR: could not unlock buf mutex\n");
        exit(1);
    }
}

void buf_init(struct buf * ctx, size_t item_size)
{
    ctx->mem = malloc(item_size * BUF_INITIAL_SIZE);
    if(!ctx->mem){
        fprintf(stderr, "ERROR: out of memory\n");
        exit(1);
    }
    ctx->item_size = item_size;

    ctx->len = 0;
    ctx->size = BUF_INITIAL_SIZE;

    if(pthread_mutex_init(& ctx->lock, NULL)){
        fprintf(stderr, "ERROR: could not initialize buf lock\n");
        exit(1);
    }
}

void buf_deinit(struct buf * ctx)
{
    if(pthread_mutex_destroy(& ctx->lock)){
        fprintf(stderr, "ERROR: could not deinitialize buf lock");
    }

    free(ctx->mem);
}

void * buf_append_begin(struct buf * ctx)
{
    lock(& ctx->lock);

    if(ctx->len == ctx->size){
        void * new = realloc(ctx->mem, ctx->item_size * ctx->size * 2);
        if(!new){
            fprintf(stderr, "ERROR: out of memory\n");
            exit(1);
        }

        ctx->mem = new;
        ctx->size *= 2;
    }

    void * ret = & ctx->mem[ctx->item_size * ctx->len];
    ctx->len += 1;

    return ret;
}

void buf_append_end(struct buf * ctx)
{
    unlock(& ctx->lock);
}

void buf_remove(struct buf * ctx, void * item)
{
    lock(& ctx->lock);

    for(size_t i=0; i<ctx->len; ++i)
    {
        if(memcmp(& ctx->mem[ctx->item_size * i], item, ctx->item_size) == 0){ // NOTE: this can be very expensive, perhaps we should be removing by index instead
            if((ctx->len >= 2) && (i != ctx->len - 1)){
                memcpy(& ctx->mem[ctx->item_size * i], & ctx->mem[ctx->item_size * (ctx->len - 1)], ctx->item_size); // NOTE: this can also be expensive, altho not as much as the memcmp of all elements up above
            }
            ctx->len -= 1;
            printf("buf_remove: removed item at index %ld\n", i);
            unlock(& ctx->lock);
            return;
        }
    }

    unlock(& ctx->lock);

    fprintf(stderr, "ERROR: could not find item in buffer\n");
}

void buf_clean(struct buf * ctx)
{
    lock(& ctx->lock);

    ctx->len = 0;

    unlock(& ctx->lock);
}

void buf_iter_begin(struct buf * ctx)
{
    lock(& ctx->lock);
}

void buf_iter_end(struct buf * ctx)
{
    unlock(& ctx->lock);
}

// needs to be used in a buf_iter context for multithreaded safety
void * buf_get(struct buf * ctx, size_t idx)
{
    return & ctx->mem[ctx->item_size * idx];
}

// needs to be used in a buf_iter context for multithreaded safety
void * buf_get_all(struct buf * ctx)
{
    return ctx->mem;
}
