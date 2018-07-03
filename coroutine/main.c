#include <stdio.h>
#include "coroutine.h"

void test1(void *args)
{
    coroutine_t *co = (coroutine_t *)args;

    for(int i = 0; i < 5; i++){
        printf("test 1 -> %d\n", i);
        core_yield(co);
    }
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
    coroutine_t *co_1 = coro_create(test1, NULL);
    coroutine_t *co_2 = coro_create(test2, NULL);
    coro_resume(co_1);
    coro_resume(co_2);
    coro_resume(co_2);
    coro_resume(co_2);
    coro_resume(co_2);
    coro_resume(co_2);
    coro_resume(co_2);
    printf("end\n");

    return 0;
}
