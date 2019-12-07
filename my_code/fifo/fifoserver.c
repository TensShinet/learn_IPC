#include "./unpipc.h"
#include "./fifo.h"


/*
打开管道的读写端

从 /tmp/fifo.serv 中读取 filename 以及 pid

读取文件内容

打开客户端的管道的读写端

向客户端的管道写文件内容
*/



int main() {
    int readfifo, writefifo, dummyfd, fd;
    char *ptr, buff[MAXLINE], fifoname[MAXLINE];
    pid_t pid;
    ssize_t n;

    // 打开 server fifo 的套路
    if ((mkfifo(SERV_FIFO, FILE_MODE) < 0) && (errno != EEXIST))
        fprintf(stderr, "can't create %s", SERV_FIFO);

    // 打开 SERV_FIFO 的读写端
    readfifo = open(SERV_FIFO, O_RDONLY, 0);
    writefifo = open(SERV_FIFO, O_WRONLY, 0);

    // 从 readfifo 中读到的文本是
    // 1234 /home/tenshine/nohup.out
    while ( (n = read(readfifo, buff, MAXLINE)) > 0) {
        // 先拿到 pid
        if (buff[n - 1] == '\n')
            n--;        /* delete newline from readline() */
        buff[n] = '\0'; /* null terminate pathname */
        
        // 找到空格
        if ((ptr = strchr(buff, ' ')) == NULL)
        {
            fprintf(stderr, "bogus request: %s", buff);
            continue;
        }

        *ptr++ = 0; /* null terminate PID, ptr = pathname */
        pid = atol(buff);
        sprintf(fifoname, "/tmp/fifo.%ld", (long)pid);

        // 打开另一个进程的 FIFO
        if ((writefifo = open(fifoname, O_WRONLY, 0)) < 0) {
            fprintf(stderr, "cannot open: %s", fifoname);
            continue;
        }


        // 打开文件内容
        if ((fd = open(ptr, O_RDONLY)) < 0)
        {
            snprintf(buff + n, sizeof(buff) - n, ": can't open, %s\n",
                     strerror(errno));
            n = strlen(ptr);
            write(writefifo, ptr, n);
            close(writefifo);
        }
        else
        {
            // 文件内容传输
            while ((n = read(fd, buff, MAXLINE)) > 0) {
                write(writefifo, buff, n);
            }
            close(fd);
            close(writefifo);
        }
    }
    return 0;
}