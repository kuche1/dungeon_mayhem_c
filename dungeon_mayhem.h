#ifndef DUNGEON_MAYHEM_H
#define DUNGEON_MAYHEM_H

#include "buf.h"
#include "net.h"

struct global_context
{
    int shutting_down;
    struct net net;
    struct buf clients;
    int clients_update_eventfd;
    pthread_t cht_thr;
};

int main(void);

#endif