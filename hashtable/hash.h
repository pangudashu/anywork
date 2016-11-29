#ifndef _HASH_H_
#define _HASH_H_

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define BUCKET_TYPE_UNDEF   1
#define BUCKET_MAX          (UINT32_MAX - 1)


typedef struct _hash_bucket Bucket;
typedef struct _hash_table  HashTable;

struct _hash_bucket {
    char            *key;
    void            *val;
    uint32_t        h;
    uint32_t        next;
    unsigned char   type;
};

struct _hash_table {
    uint32_t    nTableSize;     //哈希表大小
    uint32_t    nNumOfElments;  //元素数
    uint32_t    nNumUsed;       //已用bucket数
    uint32_t    nTableMask;     //哈希掩码
    Bucket      *arData;        //bucket
};


void hash_init(HashTable* ht, uint32_t nSize);
int hash_add(HashTable* ht, char *key, void *val);

#endif
