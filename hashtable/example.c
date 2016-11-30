#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "hash.h"

int main(void)
{
    HashTable ht;
    hash_init(&ht, 3);

    int a = 118;
    int b = 119;

    hash_add(&ht, "订单2355", &a);
    hash_add(&ht, "订单2399", &a);
    hash_add(&ht, "订单2388", &a);
    //hash_add(&ht, "订单235", &b);


    Bucket  *find_bucket,*b2;
    find_bucket = hash_get(&ht, "订单235");
    
    //hash_del(&ht, find_bucket);

    b2 = hash_get(&ht, "订单235");
    
    hash_foreach(&ht);

    return 0;
}
