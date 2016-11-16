#include "mcs_lock.h"

int mcs_init(mcs_locker_t *locker)
{
    locker->tail = NULL;
   
    return pthread_key_create(&locker->lock_node_key, NULL);
}

void mcs_thread_add_node(mcs_locker_t *locker, mcs_lock_node_t *thread_lock_node)
{
    thread_lock_node->wait = 1;
    thread_lock_node->next = NULL;
    pthread_setspecific(locker->lock_node_key, (void *)thread_lock_node);
}

mcs_lock_node_t * mcs_thread_get_node(mcs_locker_t *locker)
{
    return (mcs_lock_node_t *)pthread_getspecific(locker->lock_node_key);
}

void mcs_lock(mcs_locker_t *locker) /*{{{*/
{
   mcs_lock_node_t  *old_tail,*thread_lock_node;

   thread_lock_node = mcs_thread_get_node(locker);
   if(NULL == thread_lock_node){
       //锁失效!
       return;
   }

   old_tail = ATOMIC_EX(&locker->tail, thread_lock_node);
   if(NULL == old_tail){
       //获得锁
       return;
   }

   thread_lock_node->wait = 1;
   old_tail->next = thread_lock_node;

   COMPILER_BARRIER();
       
   //自旋等待锁释放
   while(thread_lock_node->wait){
       CPU_PAUSE();
   }

   //获得锁
   return;
}
/*}}}*/

void mcs_unlock(mcs_locker_t *locker) /*{{{*/
{
    mcs_lock_node_t  *old_tail,*thread_lock_node;

    thread_lock_node = mcs_thread_get_node(locker);
    if(NULL == thread_lock_node){
        return;
    }

    if(NULL == thread_lock_node->next){
        if(CAS(&locker->tail, thread_lock_node, NULL)){
            //解锁成功
            return;
        }

        COMPILER_BARRIER();
        //等待竞争者将自己插入thread_lock_node->next
        //此处自旋原因：竞争者在ATOMIC_EX()失败后会将自己插入等待队列，但插入成功后、设置thread_lock_node->next前next值为NULL
        while(thread_lock_node->next == NULL){
            CPU_PAUSE();
        }
    }
    //解锁下一个等待者
    thread_lock_node->next->wait = 0;
    thread_lock_node->next = NULL;
    COMPILER_BARRIER();
}
/*}}}*/


