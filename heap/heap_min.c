#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "heap.h"

heap *heap_new()
{
    heap *h = NULL;
    
    if (!(h = malloc(sizeof(heap)))) {
        return NULL;
    }

    if (!(h->nodes = malloc((HEAP_DEFAULT_INITS_SIZE + 1) * sizeof(void *)))) {
        return NULL;
    }
    h->size = HEAP_DEFAULT_INITS_SIZE + 1;
    h->num = 1;
    return h;
}

static void heap_swap(heap *h, uint32_t i, uint32_t j)
{
    heap_node *tmp_node;

    tmp_node = h->nodes[i];
    h->nodes[i] = h->nodes[j];
    h->nodes[j] = tmp_node;
}

static void heap_up(heap *h, uint32_t index)
{
    uint32_t parent;

    if (index <= 1) {
        return;
    }

    parent = index / 2;
    //比父节点小则交换
    if (h->nodes[index]->value < h->nodes[parent]->value) {
        heap_swap(h, index, parent);
        
        heap_up(h, parent);
    }
}

heap_node *heap_min_insert(heap *h, uint32_t value, void *data)
{
    uint32_t new_size;
    void *tmp;
    heap_node *n;

    if (h->num >= h->size) {
        new_size = h->size + h->size;
        if (!(tmp = realloc(h->nodes, new_size * sizeof(void *)))) {
            return NULL;
        }
        h->nodes = tmp;
        h->size = new_size;
    }

    //alloc node
    if (!(n = malloc(sizeof(heap_node)))) {
        return NULL;
    }
    n->value = value;
    n->data = data;
    
    h->nodes[h->num] = n;
    heap_up(h, h->num);
    h->num++;
    return n;
}

heap_node *heap_top(heap *h)
{
    if (!h->nodes[1]) {
        return NULL;
    }
    return h->nodes[1];
}

int main(void)
{
    int i;
    heap_node *n;
    heap *h = heap_new();

    uint32_t data[10] = {30, 20, 10, 40, 50, 30, 90, 70, 60, 3};

    for (i = 0; i < 10 ; i++) {
        heap_min_insert(h, data[i], NULL);

        n = heap_top(h);
        printf("%d\n", n->value);
    }

    return 0;
}
