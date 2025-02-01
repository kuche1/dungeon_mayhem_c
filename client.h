#ifndef CLIENT_SERVE_H
#define CLIENT_SERVE_H

#include "buf.h"

struct client
{
    int sock;
};

void client_init(int sock, struct buf * clients);
void client_serve(int sock, struct buf * clients);

#endif