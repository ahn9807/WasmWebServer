#include "wasm_server.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

struct server_arguments {
	int message_fd;
	char *portno;
};

void server_error(char *msg)
{
	perror(msg);
	exit(1);
}

int sys_setsockopt(int __fd, int __level, int __optname, const void *__optval, socklen_t __optlen);
int sys_bind(int __fd, const struct sockaddr *__addr, socklen_t __len);
int sys_listen(int __fd, int __n);
int sys_accept(int __fd, struct sockaddr *__restrict__ __addr, socklen_t *__restrict__ __addr_len);
int sys_munmap(void *__addr, size_t __len);
void *sys_mmap(void *__addr, size_t __len, int __prot, int __flags, int __fd, off_t __offset);
int sys_pipe(int *__pipedes);
int sys_pthread_create(pthread_t *__restrict__ __newthread, const pthread_attr_t *__restrict__ __attr, void *(*__start_routine)(void *), void *__restrict__ __arg);
ssize_t sys_write(int __fd, const void *__buf, size_t __n);
ssize_t sys_read(int __fd, void *__buf, size_t __nbytes);
int sys_close(int __fd);
int sys_stat(const char *__restrict__ __file, struct stat *__restrict__ __buf);
void sys_exit(int __status);
void *sys_malloc(size_t __size);
int sys_pthread_join(pthread_t __th, void **__thread_return);
int sys_printf(const char *__restrict__ __format, ...);
void undefined();