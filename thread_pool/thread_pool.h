#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <pthread.h>


typedef struct _thread_pool_s  thread_pool_t;
typedef struct _task_queue_s   task_queue_t;
typedef struct _thread_task_s  thread_task_t;

struct _thread_task_s {
    thread_task_t   *next;
    void            *ctx;
    void            (*handler)(void *arg);
};

struct _task_queue_s {
    thread_task_t   *header;
};

struct _thread_pool_s {
    pthread_mutex_t     mtx;
    task_queue_t        queue;
    int                 waiting;
    int                 thread_num;
    pthread_cond_t      cond;
    int                 max_task;
};



int thread_pool_init(thread_pool_t *pool);
static void * ngx_thread_pool_cycle(void *arg);

#endif
