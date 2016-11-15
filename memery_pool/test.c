#include <stdlib.h>
#include <stdio.h>
#include "pool.h"

struct user{
    int     id;
    char*   name;
};

int main(void)
{
    sleep(30);
    pool_t *p;

    p = create_pool(2*1024);

    struct user *u, *u1;
    int i = 0;
    
    for(;i < 100;i++){
        u = pool_alloc(p, 10240);
        u->id = 123;
        u->name = "pangudashu";

        //pool_pfree(p, u);
        u1 = pool_alloc(p, 10240);
    }
    sleep(30);

    destroy_pool(p);

    printf("ddd pagesize:%d\n", MAX_ALLOC_FROM_POOL);
    //printf("user.id:%d name:%s\n", u->id, u->name);

    sleep(40);

    return 0;
}
