#include "unpipc.h"

#define PATH_PIDFILE "./pidfile"

int lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len)
{
    struct flock lock;

    lock.l_type = type;     /* F_RDLCK, F_WRLCK, F_UNLCK */

    // 后三个参数表示被加锁的位置，可以使整个文件，也可以是部分文件，l_whence 表示加锁的开始，l_len 表示加锁的长度
    // whence 只能取三个值，SEEK_SET 表示文件开始，SEEK_CUR 表示文件中间部分，SEEK_END 表示文件结尾
    lock.l_start = offset;  /* byte offset, relative to l_whence */
    lock.l_whence = whence; /* SEEK_SET, SEEK_CUR, SEEK_END */
    lock.l_len = len;       /* #bytes (0 means to EOF) */

    return (fcntl(fd, cmd, &lock)); /* -1 upon error */
}

int write_lock(int fd, off_t offset, int whence, off_t len)
{
    // F_SETLK 进程用它来对文件的某个区域进行加锁，如果有其他锁阻止该锁被建立，那么 fcntl() 就出错返回
    // F_WRLCK 写锁
    return lock_reg(fd, F_SETLK, F_WRLCK, offset, whence, len);
}
int main(int argc, char *argv[])
{
    int pidfd;
    char line[MAXLINE];

    pidfd = open(PATH_PIDFILE, O_RDWR | O_CREAT, FILE_MODE);


    // 请求写锁
    if (write_lock(pidfd, 0, SEEK_SET, 0) != 0) {
        // 为防止和正常的返回值混淆，系统调用并不直接返回错误码，而是将错误码放入一个名为 errno 的全局变量中。如果一个系统调用失败，你可以读出 errno 的值来确定问题所在
        if (errno == EACCES || errno == EAGAIN) {
            // 请求失败，说明已经存在了一个守护进程了
            fprintf(stderr, "unable to lock %s, is %s already running?",
                     PATH_PIDFILE, argv[0]);
            exit(-1);
        } else {
            fprintf(stderr, "unable to lock %s", PATH_PIDFILE);
            exit(-1);
        }
    
    }

    // 否则是第一次开启守护进程
    // 给文件写入自己的 pid
    sprintf(line, "%ld\n", (long)getpid());
    write(pidfd, line, strlen(line));

    // 阻塞
    getchar();

    return 0;
}