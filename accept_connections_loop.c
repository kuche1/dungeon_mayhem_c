
#include "accept_connections_loop.h"

#include "client.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

void accept_connections_loop(int * shutting_down, struct net * net, struct buf * clients, int clients_update_eventfd)
{
    while(!*shutting_down)
    {
        int sock;
        if(net_client_pickup(net, & sock)){
            continue;
        }

        printf("accept_connections_loop: client pick up (%d)\n", sock);

        client_init(sock, clients);

        {
            ssize_t ret = write(clients_update_eventfd, &(uint64_t){1}, sizeof(uint64_t));
            if(ret < 0){
                perror("ERROR: accept_connections_loop: write to eventfd failed");
                exit(1);
            }
            if(ret != sizeof(uint64_t)){
                fprintf(stderr, "ERROR: accept_connections_loop: could not write full data to eventfd\n");
                exit(1);
            }
        }

        printf("accept_connections_loop: buf len %ld\n", clients->len);
    }
}
