#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <fcntl.h> 
#include <unistd.h>

#include "redis.h"
#include "event.h"
#include "coroutine.h"

void seti_nonblocking(int fd)
{
    int opts;

    opts = fcntl(fd, F_GETFL);
    if(opts < 0){
        return;
    }
    opts = opts | O_NONBLOCK;
    if(fcntl(fd, F_SETFL, opts) < 0 ){
        return;
    }
}

void redis_on_connect(void *args)
{
    redis_client *cli = (redis_client *)args;

    printf("connect success %d\n", cli->fd);
    coro_resume(cli->co);
}

redis_client *redis_new(coroutine_t *co, char *host, int port)
{
    struct epoll_event ev;
    int ret;
    redis_client *cli;
    
    cli = malloc(sizeof(redis_client));
    
    if((cli->host = gethostbyname(host)) == NULL){
        perror("gethostbyname error!");
        return NULL;
    }

    if((cli->fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("socket create error!");
        return NULL;
    }

    cli->handler.on_connect = redis_on_connect;
    cli->co = co;

    cli->serv_addr.sin_family = AF_INET;
    cli->serv_addr.sin_port = htons(port);
    cli->serv_addr.sin_addr = *((struct in_addr *)cli->host->h_addr);
    bzero(&(cli->serv_addr.sin_zero), 8);

    seti_nonblocking(cli->fd);

    ev.data.ptr = &(cli->handler);
    ev.events = EPOLLIN|EPOLLOUT|EPOLLET;

    event_ctl(EPOLL_CTL_ADD, cli->fd, &ev);

    ret = connect(cli->fd, (struct sockaddr *)&(cli->serv_addr), sizeof(struct sockaddr));
    if(ret == 0){
        return cli;
    }else{
        if(errno != EINPROGRESS){
            perror("connect failed!");
            return NULL;
        }
    }

    core_yield(co);
    printf("connect success ret\n");
}

void redis_get(redis_client *cli, char *key)
{
}
