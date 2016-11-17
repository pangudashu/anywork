#include <stdio.h>
#include <stdlib.h>

#include "thread_pool.h"


int main(void)
{
    thread_pool_t   *pool;

    pool = malloc(sizeof(thread_pool_t));
    pool->thread_num = 5;
    pool->max_task = 1000;

    thread_pool_init(pool);

    while(1){
    }

    return 0;
}
