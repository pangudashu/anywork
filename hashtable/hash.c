#include "hash.h"
#include <stdio.h>

static uint32_t _check_size(uint32_t nSize);
static int _hash_resize(HashTable* ht);
static uint32_t _string_hash_val(char *key);


static uint32_t _check_size(uint32_t nSize) /*{{{*/
{
    nSize -= 1;
    nSize |= (nSize >> 1);
    nSize |= (nSize >> 2);
    nSize |= (nSize >> 4);
    nSize |= (nSize >> 8);
    nSize |= (nSize >> 16);
    return nSize + 1;
}
/*}}}*/

static uint32_t _string_hash_val(char *key) /*{{{*/
{
    uint32_t    hash = 0 ;
    size_t      len = strlen(key);
    char        *str = key;
        
    for (; len >= 8; len -= 8) {

        hash = ((hash << 5) + hash) + *str++;
        hash = ((hash << 5) + hash) + *str++;
        hash = ((hash << 5) + hash) + *str++;
        hash = ((hash << 5) + hash) + *str++;
        hash = ((hash << 5) + hash) + *str++;
        hash = ((hash << 5) + hash) + *str++;
        hash = ((hash << 5) + hash) + *str++;
        hash = ((hash << 5) + hash) + *str++;
    }

    switch (len) {
        case 7: hash = ((hash << 5) + hash) + *str++; /* fallthrough... */
        case 6: hash = ((hash << 5) + hash) + *str++; /* fallthrough... */
        case 5: hash = ((hash << 5) + hash) + *str++; /* fallthrough... */
        case 4: hash = ((hash << 5) + hash) + *str++; /* fallthrough... */
        case 3: hash = ((hash << 5) + hash) + *str++; /* fallthrough... */
        case 2: hash = ((hash << 5) + hash) + *str++; /* fallthrough... */
        case 1: hash = ((hash << 5) + hash) + *str++; break;
        case 0: break;
    }

    return hash | 0x80000000;
}
/*}}}*/

void hash_init(HashTable* ht, uint32_t nSize) /*{{{*/
{
    nSize = _check_size(nSize);

    uint32_t    arDataSize, maskSize, i;
    uint32_t    *arHash;
    char        *data;

    maskSize = nSize * sizeof(uint32_t);

    arDataSize = maskSize + nSize * sizeof(Bucket);
    data = (char *)malloc(arDataSize);

    ht->nTableSize = nSize;
    ht->nTableMask = ~nSize + 1;
    ht->nNumOfElments = 0;
    ht->nNumUsed = 0;
    ht->arData = (Bucket *)((char *)data + maskSize);

    memset(data, HT_INVALID_IDX, maskSize);
}
/*}}}*/

int hash_add(HashTable* ht, char *key, void *val) /*{{{*/
{
    uint32_t    h, idx, nIndex;
    Bucket      *b;
    uint32_t    *arHash;

    h = _string_hash_val(key); //获取hash值

    if(ht->nNumUsed < ht->nTableSize){ //有可用空间
        b = hash_get(ht, key);
        if(b != NULL){
            b->val = val; //update
            return HASH_SUCCESS;
        }
    }else{ //扩容
        _hash_resize(ht);
    }

    b = ht->arData + ht->nNumUsed;
    idx = ht->nNumUsed;

    b->key = key;
    b->val = val;
    b->h = h;
    b->next = HT_INVALID_IDX;
    b->type = 0;

    nIndex = h | ht->nTableMask;

    arHash = ((uint32_t *)(ht)->arData) + (int32_t)nIndex;
    b->next = *arHash;
    *arHash = idx;

    ht->nNumUsed++;
    ht->nNumOfElments++;
  
#ifdef HASH_DEBUG
    printf("hash_add key:%s h:%ld arHash:%d arData.idx:%d\n", b->key, h, nIndex, idx);
#endif
    return HASH_SUCCESS;
}
/*}}}*/

Bucket * hash_get(HashTable* ht, char *key) /*{{{*/
{
    uint32_t    h, nIndex, idx;
    Bucket      *b = NULL;

    h = _string_hash_val(key);
    nIndex = h | ht->nTableMask;
    idx = *(((uint32_t *)(ht)->arData) + (int32_t)nIndex);

    while(idx != HT_INVALID_IDX){
        b = ht->arData + idx;

        if(key == b->key){
#ifdef HASH_DEBUG
            printf("hash_get key:%s h:%ld arHash:%d arData.idx:%ld\n", key, h, nIndex, idx);
#endif
            return b;
        }
        if(h == b->h && memcmp(key, b->key, strlen(key)) == 0){
#ifdef HASH_DEBUG
            printf("hash_get key:%s h:%ld arHash:%d arData.idx:%ld\n", key, h, nIndex, idx);
#endif
            return b;
        }
        idx = b->next;
    }

    return NULL;
}
/*}}}*/

int hash_del(HashTable* ht, Bucket *bucket) /*{{{*/
{
    uint32_t    nIndex,idx;
    uint32_t    *arHash;
    Bucket      *b, *prev = NULL;

    nIndex = bucket->h | ht->nTableMask;
    arHash = ((uint32_t *)(ht)->arData) + (int32_t)nIndex;

    idx = *arHash;

    while(idx != HT_INVALID_IDX){
        b = ht->arData + idx;
        
        if(b == bucket){
            break;
        }
        idx = b->next;
        prev = b;
    }

    if(b == NULL){
        return HASH_SUCCESS;
    }
#ifdef HASH_DEBUG
    printf("hash_del nIndex:%d -> idx:%ld\n", nIndex, idx);
#endif
        
    b->type = BUCKET_TYPE_UNDEF;
    if(prev != NULL){
        prev->next = b->next;
    }else{
        *arHash = b->next;
    }

    ht->nNumOfElments--;

    return HASH_SUCCESS;
}
/*}}}*/

int hash_del_by_key(HashTable* ht, char *key) /*{{{*/
{
    uint32_t    nIndex, idx, h;
    uint32_t    *arHash;
    Bucket      *b, *prev = NULL;

    h = _string_hash_val(key);

    nIndex = h | ht->nTableMask;
    arHash = ((uint32_t *)(ht)->arData) + (int32_t)nIndex;

    idx = *arHash;

    while(idx != HT_INVALID_IDX){
        b = ht->arData + idx;
        
        if(h == b->h && memcmp(key, b->key, strlen(key)) == 0){
            break;
        }
        idx = b->next;
        prev = b;
    }

    if(b == NULL){
        return HASH_SUCCESS;
    }
#ifdef HASH_DEBUG
    printf("hash_del_by_key nIndex:%d -> idx:%ld\n", nIndex, idx);
#endif
        
    b->type = BUCKET_TYPE_UNDEF;
    if(prev != NULL){
        prev->next = b->next;
    }else{
        *arHash = b->next;
    }

    ht->nNumOfElments--;

    return HASH_SUCCESS;
}
/*}}}*/

static int _hash_resize(HashTable* ht) /*{{{*/
{
    if(ht->nNumUsed > ht->nNumOfElments + (ht->nNumOfElments >> 5)){ //additional term is there to amortize the cost of compaction
#ifdef HASH_DEBUG
        printf("compaction (only rehash)\n");
#endif
        hash_rehash(ht);
    }else if(ht->nTableSize < BUCKET_MAX){ //double the table size
        char    *new_data, *old_data = (char *)ht->arData + ((int32_t)ht->nTableMask) * sizeof(uint32_t);
        uint32_t    maskSize, arDataSize, nSize = ht->nTableSize + ht->nTableSize; //double size
        Bucket  *old_bucket = ht->arData;

        maskSize = nSize * sizeof(uint32_t);

        arDataSize = maskSize + nSize * sizeof(Bucket);
        new_data = (char *)malloc(arDataSize);

        ht->nTableSize = nSize;
        ht->nTableMask = ~nSize + 1;
        ht->arData = (Bucket *)((char *)new_data + maskSize);

        memcpy(ht->arData, old_bucket, sizeof(Bucket) * ht->nNumUsed);
        memset(new_data, HT_INVALID_IDX, maskSize);
        free(old_data);

        //重建索引
        hash_rehash(ht);
#ifdef HASH_DEBUG
        printf("double size %p %p %d maskSize:%d arDataSize:%d\n", new_data, old_data, nSize, maskSize, arDataSize);
#endif
    }else{//overflow
        return HASH_RESIZE_OVERFLOW;
    }

    return 0;
}
/*}}}*/

void hash_rehash(HashTable *ht) /*{{{*/
{
    uint32_t    nIndex, j, idx = 0;
    uint32_t    *arHash;
    Bucket      *b;

#ifdef HASH_DEBUG
    printf("=================[ReHash]================\n");
#endif
    do{
        b = ht->arData + idx;

        if(b->type != BUCKET_TYPE_UNDEF){
            nIndex = b->h | ht->nTableMask;
            arHash = (uint32_t *)(ht->arData) + (int32_t)nIndex;
            b->next = *arHash;
            *arHash = idx;
            continue;
        }

        //compaction
        j = idx + 1;
        while(j < ht->nNumUsed){
            b = ht->arData + j;
            if(b->type == BUCKET_TYPE_UNDEF){
                j++;
                continue;
            }
            //move (arData + j) to (arData + idx)
            memcpy(ht->arData + idx, b, sizeof(Bucket));
            b->type = BUCKET_TYPE_UNDEF;
#ifdef HASH_DEBUG
            printf("Move Bucket %d -> %d\n", j, idx);
#endif

            nIndex = b->h | ht->nTableMask;
            arHash = (uint32_t *)(ht->arData) + (int32_t)nIndex;
            b->next = *arHash;
            *arHash = idx;

            idx++;
            j++;
        }
        break;
    }while(++idx < ht->nNumUsed);

    ht->nNumUsed = idx;
    ht->nNumOfElments = idx;
}
/*}}}*/

void hash_foreach(HashTable *ht)
{
    uint32_t    idx, i;
    Bucket      *b;
    uint32_t    *arHash;

    printf("==============[Bucket]=============\nnTableSize:%d nNumUsed:%d nNumOfElments:%d\n", ht->nTableSize, ht->nNumUsed, ht->nNumOfElments);

    for(idx = 0; idx < ht->nNumUsed; idx++){
        b = ht->arData + idx;
        if(b->type == BUCKET_TYPE_UNDEF){
            printf("idx:%d key:BUCKET_TYPE_UNDEF\n", idx);
            continue;
        }
        printf("idx:%d key:%s val:%p\n", idx, b->key, b->val);
    }

    printf("==============[Hash]=============\n");

    for(i = 1; i <= ht->nTableSize; i++){
        arHash = (uint32_t *)(ht->arData) - i;
        if(*arHash == HT_INVALID_IDX){
            printf("%d -> HT_INVALID_IDX \n", -i);
        }else{
            printf("%d -> %d \n", -i, *arHash);
        }
    }
}


