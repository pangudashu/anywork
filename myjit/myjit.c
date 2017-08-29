#include "dynasm/dasm_proto.h"
#include "dynasm/dasm_x86.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <err.h>

Dst_DECL;

#include "myjit_x86_64.c"

#define GLOB__MAX 8

void create_my_function();

static long get_page_size()
{
    static long sz;
    if (0 == sz){
        sz = sysconf(_SC_PAGESIZE);
    }
    return sz;
}

static size_t round_up_to_page_size(size_t size)
{
    long sz = get_page_size();
    size_t remainder;
    remainder = size % sz;
    return size + sz - remainder;
}

static void *create_code_mapping(size_t size)
{
    void *ptr;
    size = round_up_to_page_size(size);
    ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (MAP_FAILED == ptr){
        err(EXIT_FAILURE, "mmap");
    }
    return ptr;
}

static void protect_code_mapping(void *addr, size_t size)
{
    int retval;
    size = round_up_to_page_size(size);
    retval = mprotect(addr, size, PROT_READ | PROT_EXEC);
    if (retval){
        err(EXIT_FAILURE, "mprotect");
    }
}

static void destroy_code_mapping(void *ptr, size_t size)
{
    int retval;
    size = round_up_to_page_size(size);
    retval = munmap(ptr, size);
    if (retval){
        err(EXIT_FAILURE, "munmap");
    }
}

int main() {
    int retval;
    void *buf;
    size_t size;
    int i;
    int nglob = GLOB__MAX;
    void **glob = (void **)malloc(nglob * sizeof(void *));
    int (*my_function)(int);
    memset(glob, 0x00, nglob * sizeof(void *));
    Dst = malloc(sizeof(*Dst));

    dasm_init(Dst, 10);
    dasm_setupglobal(Dst, glob, GLOB__MAX);
    dasm_setup(Dst, my_actionlist);

    create_my_function();

    retval = dasm_link(Dst, &size);

    printf("dasm_link returned %d\n", retval);

    buf = create_code_mapping(size);

    retval = dasm_encode(Dst, buf);

    printf("dasm_encode returned %d\n", retval);

    protect_code_mapping(buf, size);

    for (i = 0; i < nglob; i++)
        printf("glob[%d] = %p\n", i, glob[i]);
    my_function = (int (*)(int))glob[0];
    printf("going to call my_function\n");

    int a = 100;
    retval = my_function(a);
    printf("my_function has returned: %d a:%d\n", retval,a);

    destroy_code_mapping(buf, size);

    dasm_free(Dst);

    free(Dst);

    return 0;
}
