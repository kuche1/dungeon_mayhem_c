#ifndef DUNGEON_MAYHEM_H
#define DUNGEON_MAYHEM_H

#include "buf.h"

struct global_context
{
    int shutting_down;
    struct buf clients;
};

int main(void);

#endif