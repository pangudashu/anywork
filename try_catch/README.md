# C语言try-catch实现
实现比较简单，主要利用`sigsetjmp`、`siglongjmp`实现堆栈的跳转

```c
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
