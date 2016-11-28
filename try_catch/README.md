# C语言try-catch实现
实现比较简单，主要利用`sigsetjmp`、`siglongjmp`实现堆栈的跳转

```c
#include <setjmp.h>

#define try                                 \
    {                                       \
        if(sigsetjmp(bailout, 0) == 0){

#define catch                               \
        }else{                                  

#define end_try()                           \
        }                                   \
    }


#define throw_exception()   siglongjmp(bailout, 0)

```

#### Use

```c
try{
    //...
    //...

    printf("call success\n");
}catch{
    printf("catch exception\n");
}end_try();
```
