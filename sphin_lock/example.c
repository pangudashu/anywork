#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "mcs_lock.h"


mcs_locker_t    *locker;


int total = 0;

void* thread_handler(void *arg)
{
    mcs_lock_node_t *node = malloc(sizeof(mcs_lock_node_t));
    int *thread_no = arg;
    int i;

    mcs_thread_add_node(locker, node);

    for(i = 0; i < 5;i++){
        mcs_lock(locker);
        total = *thread_no;
        usleep(2);
        printf("no:%d total:%d\n",*thread_no, total);
        mcs_unlock(locker);
    }
}

int main()
{
    locker = malloc(sizeof(mcs_locker_t));
    mcs_init(locker);

    pthread_t   p_list[5];
    int *arg;
    int n;

    for(n = 0; n < 5; n++){
        arg = malloc(sizeof(int));
        *arg = n;
        pthread_create(&p_list[n], NULL, thread_handler, arg);
    }

    while(1){
        sleep(1);
    }


    return 0;
}
