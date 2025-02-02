
#include "client_handler_thread.h"

#include "client.h"

#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <poll.h>

#define CHT_POLL_TIMEOUT_MS -1 // negative for infinite timeout

static void * client_handler_thread(void * voidp_args);

int client_handler_thread_spawn(struct global_context * g_ctx)
{
    if(pthread_create(& g_ctx->cht_thr, NULL, client_handler_thread, g_ctx)){
        fprintf(stderr, "ERROR: could not spawn thread\n");
        return 1;
    }

    return 0;
}

void client_handler_thread_join(pthread_t thr)
{
    if(pthread_join(thr, NULL)){
        fprintf(stderr, "ERROR: could not join thread\n");
    }
}

static int poll_sameas_interface(void * poll_a, void * poll_b, __attribute__ ((unused)) size_t item_size)
{
    struct pollfd * pa = poll_a;
    struct pollfd * pb = poll_b;
    return pa->fd == pb->fd;
}

static void * client_handler_thread(void * voidp_args)
{
    struct global_context * g_ctx = voidp_args;

    struct buf polls;
    buf_init(& polls, sizeof(struct pollfd), poll_sameas_interface);

    {
        // it's a good idea to keep this as the very first item
        // that means that when the update signal is sent we can
        // catch it right away and break out of the loop
        struct pollfd * pol_update = buf_append(& polls);
        pol_update->fd = g_ctx->clients_update_eventfd;
        pol_update->events = POLLIN;
    }

    while(!g_ctx->shutting_down)
    {
        printf("client_handler_thread: waiting for event\n");

        int num_fds = poll(buf_get_all(& polls), polls.len, CHT_POLL_TIMEOUT_MS); // TODO check if the other poll function is cooler

        if(num_fds == 0){
            // timeout
            printf("client_handler_thread: poll timeout\n");
            continue;
        }else if(num_fds < 0){
            // error
            perror("ERROR: client_handler_thread: poll");
            continue;
        }

        printf("client_handler_thread: num_fds %d\n", num_fds);

        for(size_t fd_idx=0; (fd_idx<polls.len) && (num_fds > 0); ++fd_idx){
            struct pollfd * pol = buf_get(& polls, fd_idx);

            if(pol->revents){

                int fd = pol->fd;

                if(fd == g_ctx->clients_update_eventfd){

                    printf("client_handler_thread: event on update fd (%d)\n", g_ctx->clients_update_eventfd);

                    struct client * client;
                    ssize_t ret = read(fd, & client, sizeof(client));
                    if(ret < 0){
                        perror("ERROR: could not read from eventfd");
                    }
                    if(ret != sizeof(client)){
                        fprintf(stderr, "ERROR: could not fully read from eventfd\n");
                    }

                    struct pollfd * new_pol = buf_append(& polls);
                    new_pol->fd = client->sock; // it IS valid to set fd to <0 (its just going to get ignored)
                    new_pol->events = POLLIN;

                }else{

                    printf("client_handler_thread: event on fd %d\n", fd);

                    client_serve(fd, & g_ctx->clients, & polls, fd_idx);

                }

                // num_fds -= 1;
                // TODO uncomment when sure that everything here works
            }
        }
    }

    printf("client_handler_thread: done\n");

    return NULL;
}
