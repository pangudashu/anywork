# C语言try-catch实现
实现比较简单，主要利用`sigsetjmp`、`siglongjmp`实现堆栈的跳转


setjmp()/longjmp()  和 sigsetjmp()/siglongjmp()  都是非局部跳转函数(goto 是局部跳转语句，只能在函数内跳转)，两者功能类似，不同的地方是 sigsetjmp() 使用的第一个参数类型是 sigjmp_buf (setjmp() 使用的是 jmp_buf )。此外，sigsetjmp() 还使用了第 2 个参数 savesigs ，如果该参数非零，那么在调用 sigsetjmp() 时所设置的信号掩码会保存在 env 参数中，这样使用 siglongjmp() 返回时使用相同的 env 参数就能还原为之前的信号掩码。如果 savesigs 为 0，那么信号掩码就既不会保存也不会还原。

[sigsetjmp()/siglongjmp()](http://www.ibm.com/support/knowledgecenter/SSLTBW_2.1.0/com.ibm.zos.v2r1.bpxbd00/rtsigsj.htm)

PHP try-catch也是这么实现的，具体代码:zend.h #223

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
