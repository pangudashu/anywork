#ifndef _REDIS_H
#define _REDIS_H

#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "event.h"
#include "coroutine.h"

#define READIS_STATUS_CONNECTED 0
#define READIS_STATUS_ERROR 1

typedef struct {
    event_handler handler;

    coroutine_t *co;
    int fd;
    int port;
    struct sockaddr_in  serv_addr;
    struct hostent  *host;
    
    int status;
    char *cmd;
    int cmd_send_len;
} redis_client;


redis_client *redis_new(coroutine_t *co, char *host, int port);
void redis_get(redis_client *cli, char *key);

#endif
