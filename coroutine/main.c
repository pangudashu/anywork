#include <stdio.h>
#include <unistd.h>

#include "coroutine.h"
#include "redis.h"
#include "event.h"


void test1(void *args)
{
    coroutine_t *co = (coroutine_t *)args;

    redis_client *cli = NULL;
    cli = redis_new(co, "10.94.112.246", 6300);

    if(cli == NULL){
        return;
    }
    
    redis_get(cli, "qp_coro_test");
}

void test2(void *args)
{
    coroutine_t *co = (coroutine_t *)args;
    
    redis_client *cli = NULL;
    cli = redis_new(co, "10.94.112.246", 6300);

    if(cli == NULL){
        return;
    }
    
    redis_get(cli, "qp_coro_test2");
}

int main(void)
{
    event_create();

    coroutine_t *co_2 = coro_create(test2, NULL);
    coroutine_t *co_1 = coro_create(test1, NULL);
    

    event_wait();
    return 0;
}
