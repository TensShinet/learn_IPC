#include "unpipc.h"

int main(int argc, char **argv)
{
    mqd_t mqd;
    ssize_t n;
    unsigned int  prio;
    void *buff;
    struct mq_attr attr;

    mqd = mq_open(argv[1], O_RDONLY);
    if(mqd == -1) {
        perror("mq_open");
    }
    mq_getattr(mqd, &attr);

    buff = malloc(attr.mq_msgsize);

    n = mq_receive(mqd, buff, attr.mq_msgsize, &prio);
    printf("read %ld bytes, priority = %u mq_msgsize = %ld\n", (long)n, prio, attr.mq_msgsize);
    printf("read cotent: %s", (char *)buff);

    mq_close(mqd);

    return 0;
}
