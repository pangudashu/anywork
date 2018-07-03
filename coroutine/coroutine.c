#include <stdlib.h>
#include "coroutine.h"

void coro_run(coroutine_t *co)
{
    co->func(co);
    co->end = 1;

    jump_context(&co->ctx, co->prev_ctx, co);
}

coroutine_t *coro_create(coro_func func, void *args)
{
    size_t statck_size = 16*1024;
    coroutine_t *co = (coroutine_t*)malloc(sizeof(coroutine_t) + statck_size);
    co->func = func;
    co->args = args;

    co->ctx = make_context(co->stack + statck_size, coro_run);
    
    jump_context(&co->prev_ctx, co->ctx, co);

    return co;
}

void core_yield(coroutine_t *co)
{
    jump_context(&co->ctx, co->prev_ctx, co);
}

void coro_resume(coroutine_t *co)
{
    if(co->end){
        free(co);
        return;
    }
    jump_context(&co->prev_ctx, co->ctx, co);
}

