#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "hash.h"

int main(void)
{
    Bucket  b;
    char    *key = malloc(20);
    int     val = 998;

    strcpy(key, "范德萨范德萨发");

    b.key = key;
    b.type = BUCKET_TYPE_UNDEF;

    HashTable ht;

    hash_init(&ht, 112);

    hash_add(&ht, key, &val);


    return 0;
}
