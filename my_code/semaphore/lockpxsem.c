#include "unpipc.h"
#define LOCK_PATH "/tmp/pxsemlock"

sem_t *locksem;
int initflag;

void my_lock(int fd)
{
    if (initflag == 0)
    {
        locksem = sem_open(LOCK_PATH, O_CREAT, FILE_MODE, 1);
        initflag = 1;
    }
    sem_wait(locksem);
}

void my_unlock(int fd)
{
    sem_post(locksem);
}