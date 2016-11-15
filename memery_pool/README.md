# 内存池
nginx内存池的实现，实现比较简单，非线程安全，适合阶段性比较明显的场景

![struct](https://raw.githubusercontent.com/pangudashu/anywork/master/_img/memery_pool.jpg)

### 小块内存
小内存直接在pool上分配，当前pool可用空间不够时将新分配一个pool，向操作系统申请内存，pool之间为单向链表

current指向当前可分配的pool，开始时指向第一个pool，如果当前pool分配失败次数超过3次则current后移

向操作系统申请pool内存时按页对齐，用户分配内存按sizeof(unsigned int)大小对齐

小内存不主动释放，直到pool销毁才能得到释放

### 大块内存
大内存直接向操作系统申请，pool->large指向第一块内存，多个内存结构为单链表，释放时从头遍历
