#ifndef CLIENT_HANDLER_THREAD
#define CLIENT_HANDLER_THREAD

#include "dungeon_mayhem.h"
#include "buf.h"

#include <pthread.h>

int client_handler_thread_spawn(struct global_context * g_ctx);
void client_handler_thread_join(pthread_t thr);

#endif