#include <stdlib.h>
#include <stdio.h>
#include "pool.h"


int main(void)
{
    pool_t *p;

    p = create_pool(8*1024);

    printf("ddd pagesize:%d\n", MAX_ALLOC_FROM_POOL);
    return 0;
}
