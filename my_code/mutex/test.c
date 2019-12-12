#include "./unpipc.h"
#include <unistd.h>

struct
{
    // 互斥锁
    pthread_mutex_t mutex;
} shared = {PTHREAD_MUTEX_INITIALIZER};

void thraedA() {

    pthread_mutex_lock(&shared.mutex);
    for(;;) {
        printf("A\n");
    }
    pthread_mutex_unlock(&shared.mutex);
}

void thraedB() {
    pthread_mutex_lock(&shared.mutex);
    for (;;) {
        printf("B\n");
    }
    pthread_mutex_unlock(&shared.mutex);
}

int main() {

    pthread_t A, B;
    pthread_create(&A, NULL, thraedA, NULL);
    pthread_create(&B, NULL, thraedB, NULL);


    pthread_join(A, NULL);
    pthread_join(B, NULL);
    return 0;
}