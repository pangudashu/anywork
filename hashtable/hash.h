#ifndef _HASH_H_
#define _HASH_H_

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define BUCKET_TYPE_UNDEF   1
#define BUCKET_MAX          0x04000000
#define HT_INVALID_IDX      ((uint32_t) - 1)

#define HASH_SUCCESS            0
#define HASH_RESIZE_OVERFLOW    1

//#define HASH_DEBUG

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
Bucket * hash_get(HashTable* ht, char *key);
int hash_del(HashTable* ht, Bucket *bucket);
int hash_del_by_key(HashTable* ht, char *key);
void hash_foreach(HashTable *ht);
void hash_rehash(HashTable *ht);


#endif
