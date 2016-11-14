#ifndef _POOL_H_
#define _POOL_H_

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <malloc.h>

#define POOL_ALIGNMENT  16  //申请内存块时字节对齐长度
#define ALIGNMENT   sizeof(unsigned long)  //内存对齐长度
#define DEFAULT_POOL_SIZE   16*1024
#define MAX_ALLOC_FROM_POOL (getpagesize() - 1)

#define align_ptr(p, a)                                                   \
    (u_char *) (((uintptr_t) (p) + ((uintptr_t) a - 1)) & ~((uintptr_t) a - 1))

typedef unsigned char u_char;

typedef struct _pool_small_data_s  pool_small_data_t;
typedef struct _pool_large_data_s  pool_large_data_t;
typedef struct _pool_s  pool_t;

struct _pool_small_data_s{
    u_char             *last;
    u_char             *end;
    pool_t             *next;  //指向下一个内存块
    unsigned int       failed;
};

struct _pool_large_data_s{
    pool_large_data_t  *next;
    void               *data;
};

struct _pool_s{
    pool_small_data_t   d;
    pool_large_data_t   *large;
    pool_t              *current;//当前可用pool
    size_t              max;
};


pool_t* create_pool(size_t size);
void* pool_alloc(pool_t *pool, size_t size);
void* alloc_block(pool_t *pool, size_t size);

#endif
