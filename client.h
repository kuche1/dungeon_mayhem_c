#ifndef CLIENT_SERVE_H
#define CLIENT_SERVE_H

#include "buf.h"

struct client
{
    int sock;
};

void client_init(int sock, struct buf * clients, struct client * * client);

int client_sameas_interface(void * client_a, void * client_b, size_t item_size);

void client_serve(int sock, struct buf * clients, struct buf * polls, size_t sock_idx_in_polls);

#endif