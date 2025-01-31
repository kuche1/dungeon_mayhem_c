#ifndef NET_H
#define NET_H

#include <stdint.h>
#include <netdb.h> 

struct net
{
    int sock;
    struct sockaddr_in addr;
};

[[nodiscard]] int net_init(struct net * ctx, uint16_t port, int listen_queue);
void net_deinit(struct net * ctx);

[[nodiscard]] int net_client_pickup(struct net * ctx, int * sock);
void net_client_hangup(int sock);

#endif