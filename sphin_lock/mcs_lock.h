#ifndef _MCS_LOCK_H_
#define _MCS_LOCK_H_

#include <pthread.h>

/**
 * __asm__ 告诉编译器此处插入汇编语句
 * __volatile__ 用于告诉编译器，严禁将此处的汇编语句与其它的语句重组合优化
 * memory强制gcc编译器假设RAM所有内存单元均被汇编指令修改，这样cpu中的registers和cache中已缓存的内存单元中的数据将作废
 */
#define ATOMIC_EX(old, new) __atomic_exchange_n(old, new, __ATOMIC_SEQ_CST); //REQUEST:gcc 4.7+ 原子更改old地址的值，返回更改前的值
#define CAS(lock, old, new)   __sync_bool_compare_and_swap(lock, old, new)
#define COMPILER_BARRIER() __asm__ __volatile__("" : : : "memory") 

#if ( __i386__ || __i386 || __amd64__ || __amd64 )
#define CPU_PAUSE()             __asm__ ("pause")
#else
#define CPU_PAUSE()
#endif


typedef struct _mcs_lock_node_s mcs_lock_node_t;
typedef struct _mcs_locker_s    mcs_locker_t;

//每个thread分配一个mcs_lock_node_t,各线程只对自己node的wait自旋
struct _mcs_lock_node_s {
    volatile int                wait;
    volatile mcs_lock_node_t    *next;
};


struct _mcs_locker_s {
    mcs_lock_node_t    *tail;
    pthread_key_t               lock_node_key; //记录每个thread的mcs_lock_node_t
};


int mcs_init(mcs_locker_t *locker);
void mcs_thread_add_node(mcs_locker_t *locker, mcs_lock_node_t *thread_lock_node);
mcs_lock_node_t * mcs_thread_get_node(mcs_locker_t *locker);
void mcs_lock(mcs_locker_t *locker);
void mcs_unlock(mcs_locker_t *locker);

#endif
