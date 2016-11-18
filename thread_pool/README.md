# 线程池

### 初始化
```c
thread_pool_t   *pool;
pool = malloc(sizeof(thread_pool_t));
pool->thread_num = 5;
pool->max_task = 1000;

thread_pool_init(pool);
```

### 添加任务
```c
thread_task_t   *task;
task = malloc(sizeof(thread_task_t));

task->handler = task_handler;//处理函数
task->ctx = NULL; //请求参数

thread_pool_add_task(pool, task);
```
