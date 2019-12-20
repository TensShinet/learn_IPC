#include "unpipc.h"

#define SEM_NAME "/mysem"

int main(int argc, char **argv)
{
    int fd, i, nloop, zero = 0;
    int *ptr;
    sem_t *mutex;

    if (argc != 3)
        printf("usage: incr2 <pathname> <#loops>");
    nloop = atoi(argv[2]);

    /* 4open file, initialize to 0, map into memory */
    fd = open(argv[1], O_RDWR | O_CREAT, FILE_MODE);
    // 将初值设为 0
    write(fd, &zero, sizeof(int));
    int* addr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    ptr = addr;
    close(fd);

    /* 4create, initialize, and unlink semaphore */
    mutex = sem_open(SEM_NAME, O_CREAT | O_EXCL, FILE_MODE, 1);
    if (mutex == SEM_FAILED) {
        printf("%d\n", errno);
    }
    sem_unlink(SEM_NAME);


    setbuf(stdout, NULL);

    if (fork() == 0) {
        for (i = 0; i < nloop; i++)
        {
            sem_wait(mutex);
            printf("child: %d\n", (*ptr)++);
            sem_post(mutex);
        }
        exit(0);
    }


    for (i = 0; i < nloop; i++) {
        sem_wait(mutex);
        printf("parent: %d\n", (*ptr)++);
        sem_post(mutex);
    }
    exit(0);

}