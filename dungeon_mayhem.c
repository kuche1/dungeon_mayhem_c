
#include "client_handler_thread.h"
#include "net.h"
#include "buf.h"

#include <stdio.h>

#define PORT 6969
#define LISTEN_QUEUE 5

int main(void)
{
    struct net net;
    if(net_init(& net, PORT, LISTEN_QUEUE)){
        return 1;
    }

    int shutting_down = 0;

    struct buf clients;
    buf_init(& clients);

    pthread_t cht_thr;
    if(client_handler_thread_spawn(& cht_thr, & shutting_down, & clients)){
        buf_deinit(& clients);
        net_deinit(& net);
        return 1;
    }

    while(!shutting_down)
    {
        int cli_sock;
        if(net_client_pickup(& net, & cli_sock)){
            continue;
        }

        printf("main: client pick up (%d)\n", cli_sock);

        buf_append(& clients, cli_sock);

        printf("main: buf len %ld\n", clients.len);
    }

    client_handler_thread_join(& cht_thr);

    buf_deinit(& clients);

    net_deinit(& net);

    return 0;
}
