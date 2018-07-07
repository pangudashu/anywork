#ifndef _EVENT_H
#define _EVENT_H

#include <sys/epoll.h> // for epoll_create1(), epoll_ctl(), struct epoll_event

typedef struct {
    void (*on_connect)(void *args);
    void (*on_read)(void *args);
    void (*on_write)(void *args);
} event_handler;

int event_create();
int event_ctl(int op, int fd, struct epoll_event *event);
void event_wait();


#endif
