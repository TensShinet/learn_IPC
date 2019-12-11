#include "./unpipc.h"


#define MAXNITEMS 1000000
#define MAXNTHREADS 100

int nitems;

struct {
    // 互斥锁
    pthread_mutex_t mutex;

    // 共享资源
    int buff[MAXNITEMS];
    // buff 的索引
    int nput;
    // 放进去的值
    int nvalue;
} shared = { PTHREAD_MUTEX_INITIALIZER };

/*
argv 输入 线程数 和 要产生多少 item 的数目

share.nput 记录着要写入的 buff 索引

share.nvalue 记录要写入的值
*/

void *produce(void *);
void *consume(void *);

int main(int argc, char **argv) {

    int i, nthreads, count[MAXNTHREADS];
    pthread_t tid_produce[MAXNTHREADS], tid_consume;

    if(argc != 3) {
        perror("usage:  prodcons2 <#items> <#threads>");
    }

    nitems = min(atoi(argv[1]), MAXNITEMS);
    nthreads = min(atoi(argv[2]), MAXNTHREADS);

    // 设置并发
    thr_setconcurrency(nthreads);
    // 产生所有的生产者
    for(i = 0; i < nthreads; i++) {
        count[i] = 0;
        pthread_create(&tid_produce[i], NULL, produce, &count);
    }

    // 等待所有生产者
    for(i = 0; i < nthreads; i++) {
        pthread_join(tid_produce[i], NULL);
        // 看每个线程执行了多少次
        printf("[%d]th thread = %d\n", i, count[i]);
    }

    // 创建消费者
    pthread_create(&tid_consume, NULL, consume, NULL);
    // 等待消费者
    pthread_join(tid_consume, NULL);
    return 0;
}

void *produce(void *arg) {
    for (;;) {   
        // 上锁
        pthread_mutex_lock(&shared.mutex);
        // 数组满了，解锁退出线程
        if (shared.nput >= nitems) {
            pthread_mutex_unlock(&shared.mutex);
            return (NULL);
        }
        shared.buff[shared.nput] = shared.nvalue;
        shared.nput++;
        shared.nvalue++;
        *((int *)arg) += 1;
        pthread_mutex_unlock(&shared.mutex);
    }
}
/*

如果按照我们想象的那样 

buff[i] == i

所以我们的消费者只是用来检测是否出错
*/
void *consume(void *arg) {
    int i;
    printf("nitems %d\n", nitems);
    for (i = 0; i < nitems; i++)
    {
        if (shared.buff[i] != i) {
            printf("buff[%d] = %d\n", i, shared.buff[i]);
        }
    }
    return (NULL);
}