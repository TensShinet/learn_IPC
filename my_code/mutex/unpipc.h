#include <mqueue.h>
#include <sys/types.h> /* basic system data types */
#include <sys/time.h>  /* timeval{} for select() */
#include <time.h>      /* timespec{} for pselect() */
#include <errno.h>
#include <fcntl.h>  /* for nonblocking */
#include <limits.h> /* PIPE_BUF */
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h> /* for S_xxx file mode constants */
#include <unistd.h>
#include <sys/wait.h>
#include <stdint.h>

// pthread_setconcurrency
#define __USE_UNIX98

#include <pthread.h>



#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#define MAXLINE 4096
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
