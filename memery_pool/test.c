#include <stdlib.h>
#include <stdio.h>
#include "pool.h"

struct user{
    int     id;
    char*   name;
};

int main(void)
{
    pool_t *p;

    p = create_pool(8*1024);

    struct user *u;
    u = pool_alloc(p, sizeof(struct user));
    //u->id = 123;
    //u->name = "pangudashu";

    printf("ddd pagesize:%d\n", MAX_ALLOC_FROM_POOL);
    return 0;
}
