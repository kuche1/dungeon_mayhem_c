#ifndef CLIENT_HANDLER_THREAD
#define CLIENT_HANDLER_THREAD

#include "buf.h"

#include <pthread.h>

int client_handler_thread_spawn(pthread_t * thr, int * shutting_down, struct buf * clients, int clients_update_eventfd);
void client_handler_thread_join(pthread_t thr);

#endif