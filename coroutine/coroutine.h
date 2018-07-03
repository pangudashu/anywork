#ifndef _COROUTINE_H
#define _COROUTINE_H


typedef void (*coro_func)(void *args);

typedef struct{
    coro_func   func;
    void        *args;
    int         end;
    char        *ctx;
    char        *prev_ctx;
    char        stack[0];
} coroutine_t;

char *make_context(char *stack, void (*func)(coroutine_t *co));
void jump_context(char **curr_ctx, char *new_ctx, coroutine_t *co);

coroutine_t *coro_create(coro_func func, void *args);
void core_yield(coroutine_t *co);
void coro_resume(coroutine_t *co);

#endif
