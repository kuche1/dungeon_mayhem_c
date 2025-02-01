
#include "client_handler_thread.h"
#include "net.h"
#include "buf.h"

#include <sys/eventfd.h>
#include <unistd.h>
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
    buf_init(& clients, sizeof(int));

    int clients_update_eventfd = eventfd(0, EFD_NONBLOCK);
    if(clients_update_eventfd < 0){
        perror("ERROR: eventfd");
        buf_deinit(& clients);
        net_deinit(& net);
        return 1;
    }

    pthread_t cht_thr;
    if(client_handler_thread_spawn(& cht_thr, & shutting_down, & clients, clients_update_eventfd)){
        close(clients_update_eventfd);
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

        {
            buf_lock(& clients);

            int * ptr = buf_append(& clients);
            * ptr = cli_sock;

            buf_unlock(& clients);
        }

        {
            ssize_t ret = write(clients_update_eventfd, &(uint64_t){1}, sizeof(uint64_t));
            if(ret < 0){
                perror("ERROR: write to eventfd failed");
                // TODO? clean up
                return 1;
            }
            if(ret != sizeof(uint64_t)){
                fprintf(stderr, "ERROR: could not write full data to eventfd\n");
                // TODO? clean up
                return 1;
            }
        }

        printf("main: buf len %ld\n", clients.len);
    }

    client_handler_thread_join(cht_thr);

    close(clients_update_eventfd);

    buf_deinit(& clients);

    net_deinit(& net);

    return 0;
}
