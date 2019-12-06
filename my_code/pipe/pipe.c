
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