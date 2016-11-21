#include <stdio.h>
#include <stdlib.h>

#include "thread_pool.h"


void * task_handler(void *arg)
{
    thread_pool_t   *pool = arg;

    printf("task_handler\n");
    return NULL;
}


int main(void)
{
    thread_pool_t   *pool;
    thread_task_t   *task;

    pool = malloc(sizeof(thread_pool_t));
    pool->thread_num = 5;
    pool->max_task = 1000;

    thread_pool_init(pool);

        
    for(;;){
        task = malloc(sizeof(thread_task_t));
        task->handler = task_handler;
        task->ctx = NULL;

        thread_pool_add_task(pool, task);
        sleep(1);
    }

    return 0;
}
