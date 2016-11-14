#include "pool.h"

pool_t* create_pool(size_t size)
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

void* pool_alloc(pool_t *pool, size_t size)
{
    u_char *m;
    pool_t *p;
    //小内存直接在pool上分配
    if(size <= p->max){
        p = pool->current;

        do{
            m = align_ptr(p->d.last, ALIGNMENT);
            //当前pool空间够用
            if(size_t(p->d.end - m) <= size){
                p->d.last = m + size;
                return m;
            }

            p = p->d.next;
        }while(p);

        //无可用pool,新分配
        return alloc_block(pool, size);
    }

    //todo:large memery
    return NULL;
}

//新分配内存块
void* alloc_block(pool_t *pool, size_t size)
{
    pool_t  *p,*new;
    size_t  psize;
    u_char  *m;

    psize = (size_t) (pool->end - (u_char *)pool);

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
    new->last = m + size;

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


