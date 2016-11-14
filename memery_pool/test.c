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

    p = create_pool(2*1024);

    struct user *u;
    int i;
    for(i = 0; i < 3;i++){
        u = pool_alloc(p, 1024);
        u->id = 123;
        u->name = "pangudashu";
    }

    printf("ddd pagesize:%d\n", MAX_ALLOC_FROM_POOL);
    printf("user.id:%d name:%s\n", u->id, u->name);
    return 0;
}
