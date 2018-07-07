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

    cli->status = READIS_STATUS_CONNECTED;
    coro_resume(cli->co);
}

void redis_on_write(void *args)
{
    redis_client *cli = (redis_client *)args;

    int len = send(cli->fd, cli->cmd + cli->cmd_send_len, strlen(cli->cmd) - cli->cmd_send_len, 0);

    cli->cmd_send_len += len;
    if(cli->cmd_send_len == strlen(cli->cmd)){
        coro_resume(cli->co);
    }
}

void redis_on_read(void *args)
{
    char buffer[1024];
    int ret;

    redis_client *cli = (redis_client *)args;
    
    if((ret = read(cli->fd, buffer, 1024)) < 0){
        cli->status = READIS_STATUS_ERROR;
    }
    printf("%s\n",buffer);
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

    cli->handler.on_write = redis_on_connect;
    cli->handler.on_read = redis_on_read;
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
    
    if(cli->status == READIS_STATUS_CONNECTED){
        return cli;
    }
    return NULL;
}

int redis_send_cmd(redis_client *cli, char *cmd)
{
    struct epoll_event ev;

    cli->handler.on_write = redis_on_write;
    cli->cmd = cmd;
    
    ev.data.ptr = &(cli->handler);
    ev.events = EPOLLOUT|EPOLLET;

    event_ctl(EPOLL_CTL_MOD, cli->fd, &ev);

    core_yield(cli->co);
    return 0;
}

int redis_receive(redis_client *cli)
{
    struct epoll_event ev;

    cli->handler.on_read = redis_on_read;
    
    ev.data.ptr = &(cli->handler);
    ev.events = EPOLLIN|EPOLLET;

    event_ctl(EPOLL_CTL_MOD, cli->fd, &ev);

    core_yield(cli->co);
    return 0;
}

void redis_get(redis_client *cli, char *key)
{
    int send_ret;

    char cmd[100];
    snprintf(cmd, 100, "get %s\r\n", key);

    send_ret = redis_send_cmd(cli, cmd);
    redis_receive(cli);
}
