#include "hash.h"
#include <stdio.h>

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

void hash_init(HashTable* ht, uint32_t nSize)
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

    for(i = 0; i< ht->nTableSize; i++){
        arHash = (uint32_t *)(ht->arData) - i;
        *arHash = UINT32_MAX;
    }
}

int hash_add(HashTable* ht, char *key, void *val)
{
    uint32_t    h, idx, nIndex;
    Bucket      *b;
    uint32_t    *arHash;

    h = _string_hash_val(key); //获取hash值

    if(ht->nNumUsed < ht->nTableSize){ //有可用空间
        b = ht->arData + ht->nNumUsed;
        idx = ht->nNumUsed;
    }else{ //扩容
        //...
    }

    b->key = key;
    b->val = val;
    b->h = h;
    b->next = UINT32_MAX;

    nIndex = h | ht->nTableMask;

    arHash = ((uint32_t *)(ht)->arData) + (int32_t)nIndex;
    b->next = *arHash;
    *arHash = idx;

    ht->nNumUsed++;
    ht->nNumOfElments++;
   
    //debug
    printf("hash_add key:%s h:%ld arHash:%d arData.idx:%d\n", b->key, h, nIndex, idx);
    return HASH_ADD_SUCCESS;
}

Bucket * hash_get(HashTable* ht, char *key) /*{{{*/
{
    uint32_t    h, nIndex, idx;
    Bucket      *b;

    h = _string_hash_val(key);
    nIndex = h | ht->nTableMask;
    idx = *(((uint32_t *)(ht)->arData) + (int32_t)nIndex);

    while(idx != UINT32_MAX){
        b = ht->arData + idx;

        if(key == b->key){
            return b;
        }
        if(h == b->h && memcmp(key, b->key, strlen(key)) == 0){
            return b;
        }
        idx = b->next;
    }

    //debug
    printf("hash_get key:%s h:%ld arHash:%d arData.idx:%ld\n", key, h, nIndex, idx);
    return NULL;
}
/*}}}*/



