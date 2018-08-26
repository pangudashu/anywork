
#ifndef _HEAP_H_
#define _HEAP_H_

#define HEAP_DEFAULT_INITS_SIZE 8

typedef struct _heap_node {
    uint32_t index;
    uint32_t value;
    void *data;
} heap_node;

typedef struct _heap {
    uint32_t size; //nodes总大小
    uint32_t num; //当前node数
    heap_node **nodes;
} heap;

#endif
