
#include "buf.h"

#include <stdlib.h>
#include <stdio.h>

void buf_init(struct buf * ctx)
{
    ctx->mem = malloc(sizeof(BUF_ITEM_TYPE));
    if(!ctx->mem){
        fprintf(stderr, "ERROR: out of memory\n");
        exit(1);
    }

    ctx->len = 0;
    ctx->size = 1; // this must not start as <= 0
}

void buf_append(struct buf * ctx, BUF_ITEM_TYPE item)
{
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
}

void buf_remove(struct buf * ctx, BUF_ITEM_TYPE item)
{
    if(ctx->len == 1){
        ctx->len = 0;
    }

    for(size_t i=0; i<ctx->len; ++i)
    {
        if(ctx->mem[i] == item){
            ctx->mem[i] = ctx->mem[ctx->len - 1];
            ctx->len -= 1;
            return;
        }
    }

    fprintf(stderr, "ERROR: could not find item in buffer\n");
}