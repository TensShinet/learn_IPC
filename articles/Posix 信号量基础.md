# Posix 信号量基础



> 前言：书上这一章的内容真多，一点点总结，废话不多说，让我们开始





![](http://nobathroomneeded.files.wordpress.com/2009/08/initial-d.jpg)



`今天不开车，拿停车场举个例子`



## 0X00 信号量的感性认识



之前我们学了「互斥锁」和「条件变量」，互斥锁只让某一块代码被一个多线程的一个线程执行，而条件变量可以让线程阻塞，然后一旦某个条件满足了，还可以被唤醒



`今天学习的信号量是两者的结合，可用在单纯上锁，还可用在等待唤醒`



在知乎上看到了一个信号量的解释，描述的很有画面感：



信号量就是`停车场还能停车子多少车子`，它的最大值：该停车场能停多少车子，它的当前值：如果当前值大于 0，那么是停车场还能停多少车子，如果当前值等于 0，那么代表停车场已经满了，就该去排队了



信号量有两个基本操作：等待（wait）和挂出（post）



1) 等待（wait）的意思就是，有一个车子要进来了，停车场还能停车子，值减 1。如果停车场满了，值等于 0 了，那么车子就要等待，直到大于 0，可以停车子了，然后值再减去 1



2) 挂出（post）的意思就是，有一个车子要走了，停车场多了一个位子，该值加 1，让一个等待的车子进来



`如果没有读懂我说的这个例子，建议阻塞一下，好好理解一下信号量是个啥`



说完了感性的理解，我们来看一下，「信号量」在 POSIX 标准下的分类



+ **有名信号量：使用 POSIX IPC 名字标识，可用于进程或线程间的同步**
+ **POSIX 基于内存的信号量：存放在共享内存中，可用于进程或线程间的同步**





## 0X01 信号量部分相关函数解释



**接下来我们介绍与「有名信号量」有关的函数**



+ `sem_open`  **用于创建一个新的或打开一个已经存在的有名信号量**



```c
sem_t *sem_open(const char *name, int oflag);
sem_t *sem_open(const char *name, int oflag,
                mode_t mode, unsigned int value);
```



更详细的资料见：http://man7.org/linux/man-pages/man3/sem_open.3.html



+ `sem_close` **用于关闭 sem_open 打开的信号量**



```c
#include <semaphore.h>

int sem_close(sem_t *sem);
```



一个进程终止时，内核对其上仍打开的信号量进行关闭。但是关闭不意味着删去



**所以有名信号量是随内核持续的，关闭不影响它的值**



更详细的资料见：http://man7.org/linux/man-pages/man3/sem_close.3.html





+ `sem_wait` 和 `sem_trywait` **用于测试信号量的值**



```c
#include <semaphore.h>

int sem_wait(sem_t *sem);

int sem_trywait(sem_t *sem);
```



`sem_t 是一个结构体，里面有一个成员记录信号量的值，我们把这个成员叫做 data 吧`



如果 semaphore 的 data 大于 0，那就不阻塞减 1 后立即返回；如果 semaphore 的 data  等于 0，那么就会阻塞，等待唤起以后（也就是 semaphore 的 data 大于 0 ）以后，再减 1 返回。



sem_trywait 与 sem_wait 最大的不同就是，它不会阻塞，而是返回一个错误 **EINTR**



更多详细的资料见：http://man7.org/linux/man-pages/man3/sem_timedwait.3.html



+ `sem_post 和 sem_getvalue` 



```c
#include <semaphore.h>
int sem_post(sem_t *sem);
```



`当一个线程使用完某个信号量的时候，它就应该调用 sem_post 把 semaphore 的 data 加 1，然后唤起那些由于之前 data 为 0 阻塞的进程、线程`



更多详细的资料见：http://man7.org/linux/man-pages/man3/sem_post.3.html



```c
#include <semaphore.h>
int sem_getvalue(sem_t *sem, int *sval);
```



得到信号量的值，小于 0 的绝对值就是等待该信号量解锁的线程数



更多详细的资料见：http://man7.org/linux/man-pages/man3/sem_getvalue.3.html

 



## 0X02 程序举例







### 用「信号量」实现环形缓冲区的生产者消费者问题



书上的程序稍微改了改



```c
/* include main */
#include "unpipc.h"

#define NBUFF 100
#define SEM_MUTEX "/tmp/mutex"
#define SEM_NEMPTY "/tmp/nempty"
#define SEM_NSTORED "/tmp/nstored"

int nitems; /* read-only by producer and consumer */
struct
{ /* data shared by producer and consumer */
    int buff[NBUFF];
    sem_t *mutex, *nempty, *nstored;
} shared;

void *produce(void *), *consume(void *);

int main(int argc, char **argv)
{
    pthread_t tid_produce, tid_consume;

    if (argc != 2)
        printf("usage: prodcons1 <#items>");
        exit(0);
    nitems = atoi(argv[1]);

    /* 4create three semaphores */
    shared.mutex = sem_open(SEM_MUTEX, O_CREAT | O_EXCL,
                            FILE_MODE, 1);
    shared.nempty = sem_open(SEM_NEMPTY, O_CREAT | O_EXCL,
                             FILE_MODE, NBUFF);
    shared.nstored = sem_open(SEM_NSTORED, O_CREAT | O_EXCL,
                              FILE_MODE, 0);

    /* 4create one producer thread and one consumer thread */
    pthread_setconcurrency(2);
    pthread_create(&tid_produce, NULL, produce, NULL);
    pthread_create(&tid_consume, NULL, consume, NULL);

    /* 4wait for the two threads */
    pthread_join(tid_produce, NULL);
    pthread_join(tid_consume, NULL);

    /* 4remove the semaphores */
    sem_unlink(SEM_MUTEX);
    sem_unlink(SEM_NEMPTY);
    sem_unlink(SEM_NSTORED);
    exit(0);
}
/* end main */

/* include prodcons */
void *
produce(void *arg) {
    int i;
    for (i = 0; i < nitems; i++) {
        sem_wait(shared.nempty); /* wait for at least 1 empty slot */
        sem_wait(shared.mutex);
        shared.buff[i % NBUFF] = i; /* store i into circular buffer */
        sem_post(shared.mutex);
        sem_post(shared.nstored); /* 1 more stored item */
    }
    return (NULL);
}

void *
consume(void *arg) {
    int i;

    for (i = 0; i < nitems; i++) {
        sem_wait(shared.nstored); /* wait for at least 1 stored item */
        sem_wait(shared.mutex);
        if (shared.buff[i % NBUFF] != i)
            printf("buff[%d] = %d\n", i, shared.buff[i % NBUFF]);
        sem_post(shared.mutex);
        sem_post(shared.nempty); /* 1 more empty slot */
    }
    return (NULL);
}
```



**原理就是有三个信号量，一个用来互斥，一个用来判断是不是为空，一个判断是不是为满**



### 用「信号量」给文件上锁



也是书上的程序改了改：



```c
#include "unpipc.h"
#define LOCK_PATH "/tmp/pxsemlock"


sem_t	*locksem;
int		initflag;

void my_lock(int fd)
{
	if (initflag == 0) {
		locksem = sem_open(LOCK_PATH, O_CREAT, FILE_MODE, 1);
		initflag = 1;
	}
	sem_wait(locksem);
}

void my_unlock(int fd)
{
	sem_post(locksem);
}

```



上的锁是文件劝告锁



头文件的地址：https://github.com/TensShinet/learn_IPC/blob/master/my_code/semaphore/unpipc.h















