#include <setjmp.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>


#define try                                 \
    {                                       \
        if(sigsetjmp(bailout, 0) == 0){

#define catch                               \
        }else{                                  

#define end_try()                           \
        }                                   \
    }


#define throw_exception()   siglongjmp(bailout, 0)

sigjmp_buf bailout;


int func()
{
    throw_exception();
    return 2;
}

int main()
{
    try{
        func();

        printf("call success\n");
    }catch{
        printf("catch exception\n");
    }end_try();

    return 0;
}
