#include "./unpipc.h"

int main(int argc, char **argv)
{
    mqd_t mqd;
    void *ptr;
    size_t len;
    unsigned int prio;

    if (argc != 4)
        perror("usage: mqsend <name> <#bytes> <priority>");
    len = atoi(argv[2]);
    prio = atoi(argv[3]);

    mqd = mq_open(argv[1], O_WRONLY);
    if (mqd == -1) {
        perror("mq_open");
    }

    ptr = calloc(len, sizeof(char));
    scanf("%s", (char *)ptr);

    int a = mq_send(mqd, ptr, len, prio);
    if(a == -1) {
        perror("mq_send");
    }
    mq_close(mqd);
    exit(0);
}
