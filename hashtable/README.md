# PHP哈希表(数组)实现

PHP7内部哈希表,即：php强大的array结构的实现。


```c
//zend_types.h

typedef struct _Bucket {
    zval              val;
    zend_ulong        h;                /* hash value (or numeric index)   */
    zend_string      *key;              /* string key or NULL for numerics */
} Bucket;

typedef struct _zend_array HashTable;

struct _zend_array {
    zend_refcounted_h gc;
    union {
        struct {
            ZEND_ENDIAN_LOHI_4(
                    zend_uchar    flags,
                    zend_uchar    nApplyCount,
                    zend_uchar    nIteratorsCount,
                    zend_uchar    reserve)
        } v;
        uint32_t flags;
    } u;
    uint32_t          nTableMask; //哈希值计算掩码，等于nTableSize的负值
    Bucket           *arData; //元素列表，指向第一个Bucket
    uint32_t          nNumUsed; //已用Bucket数
    uint32_t          nNumOfElements; //哈希表已有元素数
    uint32_t          nTableSize; //哈希表总大小
    uint32_t          nInternalPointer;
    zend_long         nNextFreeElement;
    dtor_func_t       pDestructor;
};
```

`Bucket`为存储元素，`arData`指向第一个Bucket；HashTable中有两个非常相近的值:`nNumUsed`、`nNumOfElements`，`nNumOfElements`表示哈希表已有元素数，那这个值不跟`nNumUsed`一样吗？为什么要定义两个呢？实际上它们有不同的含义，当将一个元素从哈希表删除时并不会将对应的Bucket移除，而是将Bucket存储的zval标示为`IS_UNDEF`，只有扩容时发现nNumOfElements与nNumUsed相差达到一定数量(这个数量是:`ht->nNumUsed - ht->nNumOfElements > (ht->nNumOfElements >> 5)`)时才会将已删除的元素全部移除，重新构建哈希表。所以`nNumUsed`>=`nNumOfElements`。


![HashTable](https://raw.githubusercontent.com/pangudashu/anywork/master/_img/ht.jpg)
