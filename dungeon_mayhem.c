
#include "net.h"

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
    while(!shutting_down)
    {
        int cli_sock;
        if(net_client_pickup(& net, & cli_sock)){
            continue;
        }

        printf("client pick up (%d)\n", cli_sock);

        net_client_hangup(cli_sock);

        printf("client hang up (%d)\n", cli_sock);
    }

    net_deinit(& net);

    return 0;
}
