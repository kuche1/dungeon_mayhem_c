
#include "client_handler_thread.h"

#include "buf.h"
#include "net.h"

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <poll.h>

#define CHT_POLL_TIMEOUT_MS 1000

static void * client_handler_thread(void * voidp_args);

int client_handler_thread_spawn(pthread_t * thr, int * shutting_down, struct buf * clients)
{
    struct cht_args * args = malloc(sizeof(* args));
    if(!args){
        fprintf(stderr, "ERROR: could not allocate memory for thread arguments\n");
        return 1;
    }

    args->shutting_down = shutting_down;
    args->clients = clients;

    if(pthread_create(thr, NULL, client_handler_thread, args)){
        fprintf(stderr, "ERROR: could not spawn thread\n");
        free(args);
        return 1;
    }

    return 0;
}

void client_handler_thread_join(pthread_t * thr)
{
    if(pthread_join(* thr, NULL)){
        fprintf(stderr, "ERROR: could not join thread\n");
    }
}

static void * client_handler_thread(void * voidp_args)
{
    struct cht_args * args = voidp_args;

    struct buf polls;
    buf_init(& polls, sizeof(struct pollfd));
    // TODO we need to add another "virtual" fd that triggeres whenever an item is added to the buffer

    while(!*args->shutting_down)
    {
        buf_clean(& polls);

        buf_iter_begin(args->clients);

        if(args->clients->len <= 0){
            printf("client_handler_thread: no clients, sleeping\n");
            buf_iter_end(args->clients);
            sleep(1);
            continue;
        }

        size_t polls_len = args->clients->len;

        for(size_t i=0; i<polls_len; ++i)
        {
            struct pollfd * pol = buf_append_begin(& polls);

            pol->fd = * (int *) buf_get(args->clients, i); // it IS valid to set fd to <0 (its just going to get ignored)
            pol->events = POLLIN;

            buf_append_end(& polls);
        }

        buf_iter_end(args->clients);

        printf("client_handler_thread: waiting for event\n");

        int num_fds = poll(buf_get_all(& polls), polls_len, CHT_POLL_TIMEOUT_MS);

        if(num_fds == 0){
            // timeout
            printf("client_handler_thread: poll timeout\n");
            continue;
        }else if(num_fds < 0){
            // error
            perror("client_handler_thread: poll");
            continue;
        }

        printf("client_handler_thread: num_fds %d\n", num_fds);

        for(size_t fd_idx=0; (fd_idx<polls_len) && (num_fds > 0); ++fd_idx){
            struct pollfd * pol = buf_get(& polls, fd_idx);

            if(pol->revents){
                int fd = pol->fd;

                printf("client_handler_thread: event on fd %d\n", fd);

                buf_remove(args->clients, & fd);

                net_client_hangup(fd);

                // num_fds -= 1;
            }
        }
    }

    free(args);

    printf("client_handler_thread: done\n");

    return NULL;
}
