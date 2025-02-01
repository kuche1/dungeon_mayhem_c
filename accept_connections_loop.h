#ifndef ACCEPT_CONNECTIONS_LOOP_H
#define ACCEPT_CONNECTIONS_LOOP_H

#include "net.h"
#include "buf.h"

void accept_connections_loop(int * shutting_down, struct net * net, struct buf * clients, int clients_update_eventfd);

#endif