# PHP哈希表(数组)实现

PHP7内部哈希表，即PHP强大array结构的内核实现。

关于哈希结构PHP7+与PHP5+的区别可以翻下[[nikic]](http://nikic.github.io/2014/12/22/PHPs-new-hashtable-implementation.html)大师早些时候写的一篇文章，这里不作讨论。

### 数据结构
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
    uint32_t          nTableMask; //哈希值计算掩码，等于nTableSize的负值(nTableMask = ~nTableSize + 1)
    Bucket           *arData; //存储元素数组，指向第一个Bucket
    uint32_t          nNumUsed; //已用Bucket数
    uint32_t          nNumOfElements; //哈希表已有元素数
    uint32_t          nTableSize; //哈希表总大小，为2的n次方
    uint32_t          nInternalPointer;
    zend_long         nNextFreeElement;
    dtor_func_t       pDestructor;
};
```

![HashTable](https://raw.githubusercontent.com/pangudashu/anywork/master/_img/ht.jpg)

HashTable中有两个非常相近的值:`nNumUsed`、`nNumOfElements`，`nNumOfElements`表示哈希表已有元素数，那这个值不跟`nNumUsed`一样吗？为什么要定义两个呢？实际上它们有不同的含义，当将一个元素从哈希表删除时并不会将对应的Bucket移除，而是将Bucket存储的zval标示为`IS_UNDEF`，只有扩容时发现nNumOfElements与nNumUsed相差达到一定数量(这个数量是:`ht->nNumUsed - ht->nNumOfElements > (ht->nNumOfElements >> 5)`)时才会将已删除的元素全部移除，重新构建哈希表。所以`nNumUsed`>=`nNumOfElements`。

HashTable中另外一个非常重要的值`arData`，这个值指向存储元素数组的第一个Bucket，插入元素时按顺序依次插入数组，比如第一个元素在arData[0]、第二个在arData[1]...arData[nNumUsed]。PHP数组的有序性正是通过`arData`保证的。

哈希表实现的关键是有一个数组存储哈希值与Bucket的映射，但是HashTable中并没有这样一个索引数组。

实际上这个索引数组包含在`arData`中，索引数组与Bucket列表一起分配，arData指向了Bucket列表的起始位置，而索引数组可以通过arData指针向前移动访问到，即arData[-1]、arData[-2]、arData[-3]......索引数组的结构是`uint32_t`,它存储的是Bucket元素在arData中的位置。

所以，整体来看HashTable主要依赖arData实现元素的存储、索引。插入一个元素时先将元素插入Bucket数组，位置是idx，再根据key的哈希值与nTableMask计算出索引数组的位置，将idx存入这个位置；查找时先根据key的哈希值与nTableMask计算出索引数组的位置，获得元素在Bucket数组的位置idx，再从Bucket数组中取出元素。

### 索引数组
索引数组类型是`uint32_t[]`，存储的值为元素在Bucket数组中的位置

索引位置(nIndex)是如何得到的？我们一般根据哈希值与数组大小取模得到，即`key->h % ht->nTableSize`，但是PHP是这么计算的：
```c
nIndex = key->h | ht->nTableMask;
```
显然位运算要比取模更快。

`nTableMask`为`nTableSize`的负数，即:`nTableMask = ~nTableSize + 1`，因为`nTableSize`等于2^n，所以`nTableMask`二进制位右侧全部为0，也就保证了`|nIndex| <= nTableSize`：
```c
11111111 11111111 11111111 11111000   -8
11111111 11111111 11111111 11110000   -16
11111111 11111111 11111111 11100000   -32
11111111 11111111 11111111 11000000   -64
11111111 11111111 11111111 10000000   -128
```
### 哈希冲突
哈希冲突是指不同的key可能计算得到相同的哈希值，但是这些值又需要插入同一个哈希表。一般解决方法是将Bucket串成链表，查找时遍历链表比较key。

PHP的实现也是类似，只是指向冲突元素的指针并没有直接存在Bucket中，而是存在嵌入的`zval`中，zval的结构：

```c
struct _zval_struct {
    zend_value        value;            /* value */
    union {
        struct {
            ZEND_ENDIAN_LOHI_4(
                    zend_uchar    type,         /* active type */
                    zend_uchar    type_flags,
                    zend_uchar    const_flags,
                    zend_uchar    reserved)     /* call info for EX(This) */
        } v;
        uint32_t type_info;
    } u1;
    union {
        uint32_t     var_flags;
        uint32_t     next;                 /* hash collision chain */
        uint32_t     cache_slot;           /* literal cache slot */
        uint32_t     lineno;               /* line number (for ast nodes) */
        uint32_t     num_args;             /* arguments number for EX(This) */
        uint32_t     fe_pos;               /* foreach position */
        uint32_t     fe_iter_idx;          /* foreach iterator index */
    } u2;
};
```

`zval.u2.next`存的就是冲突元素在Bucket数组中的位置，所以查找过程类似：

```c
zend_ulong h = zend_string_hash_val(key);
uint32_t idx = ht->arHash[h & ht->nTableMask];
while (idx != INVALID_IDX) {
    Bucket *b = &ht->arData[idx];
    if (b->h == h && zend_string_equals(b->key, key)) {
        return b;
    }
    idx = Z_NEXT(b->val); // b->val.u2.next
}
return NULL;
```
