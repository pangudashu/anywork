#include <sys/epoll.h> // for epoll_create1(), epoll_ctl(), struct epoll_event
#include <string.h>    // for strncmp
#include <stdio.h>

#include "event.h"
/*
typedef union epoll_data {
   void        *ptr;
   int          fd;
   uint32_t     u32;
   uint64_t     u64;
} epoll_data_t;

struct epoll_event {
   uint32_t     events;     
   epoll_data_t data;        
};
*/

int epfd;

int event_create()
{
    epfd = epoll_create(10);
    
    return epfd;
}

int event_ctl(int op, int fd, struct epoll_event *event)
{
    return epoll_ctl(epfd, op, fd, event); 
}

void event_wait()
{
    struct epoll_event events[10];
    int n;
        
    while(1){
        n = epoll_wait(epfd, events, 10, -1);
        for(int i = 0; i < n; i++){
            printf("act evnet:%d read:%d write:%d\n", i, events[i].events&EPOLLIN, events[i].events&EPOLLOUT);
            if(events[i].events&EPOLLIN){
                ((event_handler*)(events[i].data.ptr))->on_read(events[i].data.ptr);
            }else if(events[i].events&EPOLLOUT){
                ((event_handler*)(events[i].data.ptr))->on_connect(events[i].data.ptr);
            }
        }
    }
}

