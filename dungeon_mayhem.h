#ifndef DUNGEON_MAYHEM_H
#define DUNGEON_MAYHEM_H

#include "buf.h"
#include "net.h"

struct global_context
{
    int shutting_down;
    struct net net;
    struct buf clients;
};

int main(void);

#endif