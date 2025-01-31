
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

    while(1)
    {
        int cli_sock;
        if(net_client_pickup(& net, & cli_sock)){
            continue;
        }

        printf("client pick up\n");

        net_client_hangup(cli_sock);

        printf("client hang up\n");
    }

    net_deinit(& net);

    return 0;
}
