#include "pool.h"

pool_t* create_pool(size_t size) /*{{{*/
{
    pool_t *p;

    p = memalign(POOL_ALIGNMENT, size);
    if(p == NULL){
        return NULL;
    }

    p->d.last = (u_char *)p + sizeof(pool_t);
    p->d.end = (u_char *)p + size;
    p->d.next = NULL;
    p->d.failed = 0;

    size = size - sizeof(pool_t);
    p->max = (size < MAX_ALLOC_FROM_POOL) ? size : MAX_ALLOC_FROM_POOL;//pool分配最大值
    p->current = p;
    p->large = NULL;
    
    return p;
}
/*}}}*/

int destroy_pool(pool_t *pool) /*{{{*/
{
    pool_large_data_t   *l;
    pool_t              *p,*n;

    l = pool->large;
    while(l){
        if(l->data){
            free(l->data);
        }
        l = l->next;
    }

    p = pool;
    n = pool->d.next;

    while(p){
        free(p);
        p = n;

        if(n){
            n = n->d.next;
        }
    }

    return 1;
}
/*}}}*/

void* pool_alloc(pool_t *pool, size_t size) /*{{{*/
{
    u_char *m;
    pool_t *p;
    //小内存直接在pool上分配
    if(size <= pool->max){
        p = pool->current;

        do{
            m = align_ptr(p->d.last, ALIGNMENT);
            //当前pool空间够用
            if((size_t) (p->d.end - m) >= size){
                p->d.last = m + size;
                return m;
            }

            p = p->d.next;
        }while(p);

        //无可用pool,新分配
        return alloc_block(pool, size);
    }

    //large memery
    return alloc_large(pool, size);
}
/*}}}*/

//新分配内存块
void* alloc_block(pool_t *pool, size_t size) /*{{{*/
{
    pool_t  *p,*new;
    size_t  psize;
    u_char  *m;

    psize = (size_t) (pool->d.end - (u_char *)pool);

    m = memalign(POOL_ALIGNMENT, psize);
    if(m == NULL){
        return NULL;
    }
    new = (pool_t *) m;
    new->d.end = m + psize;
    new->d.failed = 0;
    new->d.next = NULL;

    m += sizeof(pool_t);
    m = align_ptr(m, ALIGNMENT);
    new->d.last = m + size;

    p = pool->current;
    while(p->d.next){
        if(p->d.failed++ > 4){
            pool->current = p->d.next;
        }
        p = p->d.next;
    }
    p->d.next = new;
    return m;
}
/*}}}*/

void* alloc_large(pool_t *pool, size_t size) /*{{{*/
{
    int                 n;
    void                *p;
    pool_large_data_t   *large;

    p = malloc(size);
    if(p == NULL){
        return NULL;
    }

    n = 0;
    large = pool->large;
    while(large){
        if(large->data == NULL){
            large->data = p;
            return p;
        }

        if(n++ > 3){
            break;
        }
        large = large->next;
    }

    large = pool_alloc(pool, sizeof(pool_large_data_t));
    if(large == NULL){
        free(p);
        return NULL;
    }
    
    large->data = p;
    large->next  = pool->large;
    pool->large = large;

    return p;
}
/*}}}*/

int pool_pfree(pool_t *pool, void *p) /*{{{*/
{
    pool_large_data_t   *l;

    l = pool->large;
    while(l){
        if(!l->data){
            continue;
        }
        if(p == l->data){
            free(l->data);
            l->data = NULL;
            return 1;
        }
    }
    return 1;
}
/*}}}*/
