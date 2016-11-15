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

    p = create_pool(200*1024*1024);

    struct user *u, *u1;
    int i = 0;
    
    for(;i < 100;i++){
        u = pool_alloc(p, 10240);
        u->id = 123;
        u->name = "pangudashu";

        u1 = pool_alloc(p, 10240);
    }

    destroy_pool(p);

    return 0;
}
