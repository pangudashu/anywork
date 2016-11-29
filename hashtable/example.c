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

    hash_add(&ht, "订单234屌丝夫妇", &a);
    hash_add(&ht, "订单235", &b);

    Bucket  *find_bucket;
    find_bucket = hash_get(&ht, "订单234屌丝夫妇");
    
    printf("%s\n", find_bucket->key);


    return 0;
}
