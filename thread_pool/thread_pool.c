#include "thread_pool.h"
#include <stdio.h>



int thread_pool_init(thread_pool_t *pool)
{
    pthread_t       tid;
    pthread_attr_t  attr;
    int             i;

    pthread_mutex_init(&pool->mtx, NULL);

    pool->queue.header = NULL;
    pool->queue.tail = NULL;//pool->queue.header;

    if(pthread_cond_init(&pool->cond, NULL)){
        pthread_mutex_destroy(&pool->mtx);
        return -1;
    }

    if(pthread_attr_init(&attr)){
        return -1;
    }

    for(i = 0; i < pool->thread_num; i++){
        if(pthread_create(&tid, &attr, thread_pool_cycle, pool)){
            return -1;
        }
    }

    (void) pthread_attr_destroy(&attr);

    return 0;
}

static void * thread_pool_cycle(void *arg)
{
    thread_pool_t   *pool = arg;
    thread_task_t   *task;

    while(1){
        if(pthread_mutex_lock(&pool->mtx)){
            return NULL;
        }

        while(pool->queue.header == NULL){
            if(pthread_cond_wait(&pool->cond, &pool->mtx)){
                pthread_mutex_unlock(&pool->mtx);
                return NULL;
            }
        }
        task = pool->queue.header;
        pool->queue.header = task->next;

        if(pool->queue.header == NULL){
            pool->queue.tail = NULL;
        }

        if(pthread_mutex_unlock(&pool->mtx)){
            return NULL;
        }

        //handler
        task->handler(task->ctx);
    }
}

int thread_pool_add_task(thread_pool_t *pool, thread_task_t *task)
{
    if(pthread_mutex_lock(&pool->mtx)){
        return -1;
    }

    if(pool->waiting >= pool->max_task){
        pthread_mutex_unlock(&pool->mtx);
        return -1;
    }

    if(pthread_cond_signal(&pool->cond)){
        pthread_mutex_unlock(&pool->mtx);
        return -1;
    }
    
    pool->waiting++;
    if(pool->queue.tail != NULL){
        pool->queue.tail->next = task;
        pool->queue.tail = task;
    }else{
        pool->queue.tail = task;
        pool->queue.header = task;
    }

    pthread_mutex_unlock(&pool->mtx);

    return 0;
}



