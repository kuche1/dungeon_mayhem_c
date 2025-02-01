
#include "accept_connections_loop.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

void accept_connections_loop(int * shutting_down, struct net * net, struct buf * clients, int clients_update_eventfd)
{
    while(!*shutting_down)
    {
        int cli_sock;
        if(net_client_pickup(net, & cli_sock)){
            continue;
        }

        printf("accept_connections_loop: client pick up (%d)\n", cli_sock);

        {
            buf_lock(clients);

            int * ptr = buf_append(clients);
            * ptr = cli_sock;

            buf_unlock(clients);
        }

        {
            ssize_t ret = write(clients_update_eventfd, &(uint64_t){1}, sizeof(uint64_t));
            if(ret < 0){
                perror("ERROR: write to eventfd failed");
                // TODO? clean up
                exit(1);
            }
            if(ret != sizeof(uint64_t)){
                fprintf(stderr, "ERROR: could not write full data to eventfd\n");
                // TODO? clean up
                exit(1);
            }
        }

        printf("accept_connections_loop: buf len %ld\n", clients->len);
    }
}
