#include <stdint.h>
#include <stdio.h>

#include "heap.h"


int main(void)
{
    int i;
    heap_node *n;
    heap *h = heap_new();

    uint32_t data[10] = {30, 20, 10, 40, 50, 30, 90, 70, 60, 3};

    for (i = 0; i < 10 ; i++) {
        n = heap_min_insert(h, data[i], NULL);

        n = heap_top(h);
        printf("index:%d -> %d\n", n->index, n->value);
    }

    for (i = 1; i < 10; i++) {
        n = heap_top(h);
        
        heap_min_delete(h, n);
        
        n = heap_top(h);

        printf("index:%d -> %d\n", n->index, n->value);
    }

    return 0;
}
