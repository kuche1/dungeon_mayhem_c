#ifndef BUF_H
#define BUF_H

#include <stddef.h>

#define BUF_ITEM_TYPE int

struct buf
{
    BUF_ITEM_TYPE * mem;
    size_t len;
    size_t size;
};

void buf_init(struct buf * ctx);
void buf_append(struct buf * ctx, BUF_ITEM_TYPE item);
void buf_remove(struct buf * ctx, BUF_ITEM_TYPE item);

#endif