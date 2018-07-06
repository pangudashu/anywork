#include <stdio.h>
#include <unistd.h>

#include "coroutine.h"
#include "redis.h"
#include "event.h"


void test1(void *args)
{
    coroutine_t *co = (coroutine_t *)args;

    redis_client *cli = redis_new(co, "10.94.112.246", 9999);
}

void test2(void *args)
{
    coroutine_t *co = (coroutine_t *)args;
    
    for(int i = 0; i < 5; i++){
        printf("test 2 -> %d\n", i);
        core_yield(co);
    }
}

int main(void)
{
    event_create();

    coroutine_t *co_1 = coro_create(test1, NULL);
    //coro_resume(co_1);
    
    /*
    coroutine_t *co_2 = coro_create(test2, NULL);
    coro_resume(co_2);
    coro_resume(co_2);
    coro_resume(co_2);
    coro_resume(co_2);
    coro_resume(co_2);
    coro_resume(co_2);
    */

    event_wait();
    return 0;
}
