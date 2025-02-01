
#include "dungeon_mayhem.h"

#include "accept_connections_loop.h"
#include "client_handler_thread.h"
#include "client.h"
#include "net.h"
#include "buf.h"

#include <sys/eventfd.h>
#include <unistd.h>
#include <stdio.h>

#define PORT 6969
#define LISTEN_QUEUE 5

int main(void)
{
    struct global_context g_ctx;
    g_ctx.shutting_down = 0;

    if(net_init(& g_ctx.net, PORT, LISTEN_QUEUE)){
        return 1;
    }

    buf_init(& g_ctx.clients, sizeof(struct client));

    g_ctx.clients_update_eventfd = eventfd(0, EFD_NONBLOCK);
    if(g_ctx.clients_update_eventfd < 0){
        perror("ERROR: eventfd");
        buf_deinit(& g_ctx.clients);
        net_deinit(& g_ctx.net);
        return 1;
    }

    if(client_handler_thread_spawn(& g_ctx)){
        close(g_ctx.clients_update_eventfd);
        buf_deinit(& g_ctx.clients);
        net_deinit(& g_ctx.net);
        return 1;
    }

    accept_connections_loop(& g_ctx.shutting_down, & g_ctx.net, & g_ctx.clients, g_ctx.clients_update_eventfd);

    client_handler_thread_join(g_ctx.cht_thr);

    close(g_ctx.clients_update_eventfd);

    // TODO close all client connections?
    buf_deinit(& g_ctx.clients);

    net_deinit(& g_ctx.net);

    return 0;
}
