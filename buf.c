
#include "buf.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define BUF_INITIAL_SIZE 1 // this must be at least 1

static int buf_sameas_fnc_generic(void * a, void * b, size_t item_size)
{
    return memcmp(a, b, item_size) == 0;
}

void buf_init(struct buf * ctx, size_t item_size, buf_sameas_function sameas_fnc)
{
    ctx->mem = malloc(item_size * BUF_INITIAL_SIZE);
    if(!ctx->mem){
        fprintf(stderr, "ERROR: out of memory\n");
        exit(1);
    }

    ctx->item_size = item_size;

    if(sameas_fnc){
        ctx->sameas_fnc = sameas_fnc;
    }else{
        ctx->sameas_fnc = buf_sameas_fnc_generic;
    }

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

void buf_lock(struct buf * ctx)
{
    if(pthread_mutex_lock(& ctx->lock)){
        fprintf(stderr, "ERROR: could not lock buf mutex\n");
        exit(1);
    }
}

void buf_unlock(struct buf * ctx)
{
    if(pthread_mutex_unlock(& ctx->lock)){
        fprintf(stderr, "ERROR: could not unlock buf mutex\n");
        exit(1);
    }
}

void * buf_append(struct buf * ctx)
{
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

void buf_remove(struct buf * ctx, void * item)
{
    for(size_t i=0; i<ctx->len; ++i)
    {
        if(ctx->sameas_fnc(& ctx->mem[ctx->item_size * i], item, ctx->item_size)){
            if((ctx->len >= 2) && (i != ctx->len - 1)){
                memcpy(& ctx->mem[ctx->item_size * i], & ctx->mem[ctx->item_size * (ctx->len - 1)], ctx->item_size); // TODO: this will copy the whole item, perhaps we should use a `copy_fnc`
            }
            ctx->len -= 1;
            printf("buf_remove: removed item at index %ld\n", i);
            return;
        }
    }

    fprintf(stderr, "ERROR: could not find item in buffer\n");
}

void buf_clear(struct buf * ctx, size_t items_to_keep)
{
    if(items_to_keep > ctx->len){
        fprintf(stderr, "ERROR: trying to clear everything but %ld elements in a buf of %ld elements\n", items_to_keep, ctx->len);
        return;
    }

    ctx->len = items_to_keep;
}

void * buf_get(struct buf * ctx, size_t idx)
{
    return & ctx->mem[ctx->item_size * idx];
}

void * buf_get_all(struct buf * ctx)
{
    return ctx->mem;
}
