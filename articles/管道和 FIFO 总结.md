# 管道和 FIFO 总结





> 开始系统学习进程间通信





![](../imgs/1.png)





对于进程间的通信大概可以抽象成以上三种，而我们第一次介绍的管道和 FIFO 就是第二种



## 0X00 管道通信——Pipe



![](../imgs/2.png)





如上图所示，使用 pipe 的原理就是在内核中创建一个`管道，一头用来读，一头用来写`。虽然管道是单进程创建的，但是管道的典型用途是`兄弟，父子`进程的通信手段之一：





![](../imgs/3.png)



在 c 语言中，使用：



```c
    int p[2];
    pipe(p);
```



p[0], p[1] 会存放着两个文件描述符。其中`p[0] 用来读，p[1] 用来写`





接下来我们用 c 实现下面的程序，书上的原 c 程序在这里：https://github.com/TensShinet/learn_IPC/blob/master/book_code/pipe/mainpipe.c





![](../imgs/4.png)





```c

#include "./pipe.h"

void server(int readfd, int writefd) {
	// 子进程
	// 使用 popen + cat 读取文件
	size_t len;
	ssize_t n;
	char buff[MAXLINE+1];
	char command[MAXLINE];

	// 读取文件名
	if ((n = read(readfd, buff, MAXLINE)) == 0) {}
	buff[n] = '\0';
	sprintf(command, "cat %s", buff);

	// 使用 popen 读取文件
	FILE *catfp = popen(command, "r");
	while (fgets(buff, MAXLINE + 1, catfp) != NULL) {
		write(writefd, buff, strlen(buff));
	}

	pclose(catfp);
}

void client(int readfd, int writefd) {
	// 父进程
	// 输入路径
	// 将得到的文件内容写入输出流
	size_t len;
	ssize_t n;
	char buff[MAXLINE];

	fputs("file path:", stdout);
	fgets(buff, MAXLINE, stdin);
	len = strlen(buff);

	if(buff[len-1] == '\n') {
		len--;
	}

	// 写入文件路径
	write(writefd, buff, len);

	// 写入输出
	while ((n = read(readfd, buff, MAXLINE)) > 0) {
		write(1, buff, n);
	}
}

int main() {

	int pipe1[2], pipe2[2];

	// 创建两个 pipe
	pipe(pipe1);
	pipe(pipe2);

	pid_t child_pid;
	if((child_pid = fork()) == 0) {
		// 子进程
		close(pipe1[1]);
		close(pipe2[0]);
		server(pipe1[0], pipe2[1]);
	} else
	
	{
		// 父进程
		close(pipe2[1]);
		close(pipe1[0]);
		client(pipe2[0], pipe1[1]);
		waitpid(child_pid, NULL, 0);
	}
	

	return 0;
}
```



有关头文件的代码在这里：















## 0X01 FIFO











 



