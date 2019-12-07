#include "./unpipc.h"
#include "./fifo.h"

/*
打开 FIFO server 的读写端

得到自己的 pid

输入路径

创建自己的 FIFO

将 pid 与路径结合,写入 server FIFO 写端

删除自己的 FIFO
*/
int main() {
    int readfifo, writefifo, dummyfd, fd;
    char *ptr, buff[MAXLINE], fifoname[MAXLINE], path[MAXLINE];
    pid_t pid;
    ssize_t n;


    // 得到自己的 pid
    pid = getpid();
    sprintf(fifoname, "/tmp/fifo.%ld", (long)pid);


    // 创建自己的 FIFO
    if ((mkfifo(fifoname, FILE_MODE) < 0) && (errno != EEXIST))
        fprintf(stderr, "can't create %s", fifoname);


    fputs("file path:", stdout);
    // 输入路径
    fgets(path, MAXLINE, stdin);


    // 打开服务器的写端
    writefifo = open(SERV_FIFO, O_WRONLY, 0);

    // 写入 pid 和路径
    sprintf(buff, "%ld %s", (long)pid, path);

    // 写入写端
    write(writefifo, buff, strlen(buff));

    // 打开读端
    readfifo = open(fifoname, O_RDONLY, 0);

    // 读数据
    // int goalfd = open("./test.log", O_WRONLY, 0);
    while((n = read(readfifo, buff, MAXLINE)) > 0) {
        
        write(1, buff, n);
    }

    // close(goalfd);
    close(readfifo);
    // 删除自己的 FIFO
    unlink(fifoname);

    return 0;
}