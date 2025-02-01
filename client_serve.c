
#include "client_serve.h"

#include "net.h"

#include <stdio.h>

void client_serve(int sock, struct buf * clients)
{
    printf("client_serve\n");

    {
        buf_lock(clients);
        buf_remove(clients, & sock);
        buf_unlock(clients);
    }

    net_client_hangup(sock);
}
