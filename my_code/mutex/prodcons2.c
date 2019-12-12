#include "./unpipc.h"

#define MAXNITEMS 1000000
#define MAXNTHREADS 100

int nitems = 0;

struct
{
    // 互斥锁
    pthread_mutex_t mutex;

    // 共享资源
    int buff[MAXNITEMS];
    // buff 的索引
    int nput;
    // 放进去的值
    int nvalue;
} put = {PTHREAD_MUTEX_INITIALIZER};

struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int nready; /* number ready for consumer */
} nready = {PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER};

/*
argv 输入 线程数 和 要产生多少 item 的数目

share.nput 记录着要写入的 buff 索引

share.nvalue 记录要写入的值

条件变量的结构体设置三个值

mutex 用于锁
cond 用于条件满足的 signal
nready 如果生产者产生了一个 item nready++ 然后设置条件
消费者线程被唤醒，执行操作以后，nready--
*/

void *produce(void *);
void *consume(void *);

int main(int argc, char **argv)
{
    int i, nthreads, count[MAXNTHREADS];
    pthread_t tid_produce[MAXNTHREADS], tid_consume;

    if (argc != 3)
    {
        printf("usage:  prodcons2 <#items> <#threads>");
        return 0;
    }

    nitems = min(atoi(argv[1]), MAXNITEMS);
    nthreads = min(atoi(argv[2]), MAXNTHREADS);

    // 设置并发
    // pthread_setconcurrency(nthreads + 1);
    // 产生所有的生产者
    for (i = 0; i < nthreads; i++)
    {
        count[i] = 0;
        pthread_create(&tid_produce[i], NULL, produce, &count);
    }
    // 创建消费者
    pthread_create(&tid_consume, NULL, consume, NULL);

    for (i = 0; i < nthreads; i++)
    {   
        pthread_join(tid_produce[i], NULL);
        // 看每个线程执行了多少次
        printf("[%d]th thread = %d\n", i, count[i]);
    }
    // 等待消费者
    pthread_join(tid_consume, NULL);
    return 0;
}

void *produce(void *arg)
{

    for (;;)
    {
        // 上锁
        pthread_mutex_lock(&put.mutex);
        // 数组满了，解锁退出线程
        if (put.nput >= nitems)
        {
            pthread_mutex_unlock(&put.mutex);
            return (NULL);
        }
        put.buff[put.nput] = put.nvalue;
        put.nput++;
        put.nvalue++;
        *((int *)arg) += 1;
        pthread_mutex_unlock(&put.mutex);

        // 完成一次操作
        pthread_mutex_lock(&nready.mutex);
        if (nready.nready == 0)
            // 唤醒另外一个线程
            pthread_cond_signal(&nready.cond);
        nready.nready++;
        pthread_mutex_unlock(&nready.mutex);
    }
}
/*

如果按照我们想象的那样 

buff[i] == i

所以我们的消费者只是用来检测是否出错
*/
void *consume(void *arg)
{
    int i;
    for (i = 0; i < nitems; i++)
    {
        pthread_mutex_lock(&nready.mutex);
        while (nready.nready == 0)
            pthread_cond_wait(&nready.cond, &nready.mutex);
        nready.nready--;
        pthread_mutex_unlock(&nready.mutex);

        if (put.buff[i] != i)
        {
            printf("buff[%d] = %d\n", i, put.buff[i]);
        }
    }
    return (NULL);
}