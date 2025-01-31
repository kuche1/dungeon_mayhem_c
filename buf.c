
#include "buf.h"

#include <stdlib.h>
#include <stdio.h>

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

void buf_init(struct buf * ctx)
{
    ctx->mem = malloc(sizeof(BUF_ITEM_TYPE));
    if(!ctx->mem){
        fprintf(stderr, "ERROR: out of memory\n");
        exit(1);
    }

    ctx->len = 0;
    ctx->size = 1; // this must not start as <= 0

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
}

void buf_append(struct buf * ctx, BUF_ITEM_TYPE item)
{
    lock(& ctx->lock);

    if(ctx->len == ctx->size){
        void * new = realloc(ctx->mem, sizeof(BUF_ITEM_TYPE) * ctx->size * 2);
        if(!new){
            fprintf(stderr, "ERROR: out of memory\n");
            exit(1);
        }

        ctx->mem = new;
        ctx->size *= 2;
    }

    ctx->mem[ctx->len] = item;
    ctx->len += 1;

    unlock(& ctx->lock);
}

void buf_remove(struct buf * ctx, BUF_ITEM_TYPE item)
{
    lock(& ctx->lock);

    if(ctx->len == 1){
        ctx->len = 0;
        unlock(& ctx->lock);
        return;
    }

    for(size_t i=0; i<ctx->len; ++i)
    {
        if(ctx->mem[i] == item){
            ctx->mem[i] = ctx->mem[ctx->len - 1];
            ctx->len -= 1;
            return;
        }
    }

    unlock(& ctx->lock);

    fprintf(stderr, "ERROR: could not find item in buffer\n");
}