#ifndef CLIENT_SERVE_H
#define CLIENT_SERVE_H

#include "buf.h"

void client_serve(int sock, struct buf * clients);

#endif