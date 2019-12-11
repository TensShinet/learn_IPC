#include "./unpipc.h"

int main(int argc, char **argv)
{
    int c, flags;
    mqd_t mqd;

    if (argc != 2)
        perror("usage: mqcreate  <name>");

    flags = O_RDWR | O_CREAT;
    mqd = mq_open(argv[1], flags, FILE_MODE, NULL);

    if(mqd == -1) {
        perror("mq_open fails");
    }
    // 关闭消息队列
    mq_close(mqd);
    // 删除消息队列
    int a = mq_unlink(argv[1]);
    if(a) {
        perror("mq_unlink fails");
    }
    return 0;
}
