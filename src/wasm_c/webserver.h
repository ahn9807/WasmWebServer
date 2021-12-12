#pragma once

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

#define BUFSIZE 1024

struct server_arguments {
	int message_fd;
	char *portno;
};

void cerror(FILE *stream, char *cause, char *errno_in,
			char *shortmsg, char *longmsg);

void server_error(char *msg)
{
	sys_printf("panic: %s\n", msg);
	exit(1);
}

__attribute__((used))
void * server_main(void *);
__attribute__((used))
void * server_worker(void *);

extern int sys_socket(int __domain, int __type, int __protocol);

extern int sys_setsockopt(int __fd, int __level, int __optname, const void *__optval, socklen_t __optlen);
extern int sys_bind(int __fd, const struct sockaddr *__addr, socklen_t __len);
extern int sys_listen(int __fd, int __n);
extern int sys_accept(int __fd, struct sockaddr *__restrict__ __addr, socklen_t *__restrict__ __addr_len);
extern int sys_munmap(void *__addr, size_t __len);
extern void *sys_mmap(void *__addr, size_t __len, int __prot, int __flags, int __fd, off_t __offset);
extern int sys_pipe(int *__pipedes);
extern int sys_pthread_create(pthread_t *__restrict__ __newthread, const pthread_attr_t *__restrict__ __attr, void *(*__start_routine)(void *), void *__restrict__ __arg);
extern ssize_t sys_write(int __fd, const void *__buf, size_t __n);
extern ssize_t sys_read(int __fd, void *__buf, size_t __nbytes);
extern int sys_close(int __fd);
extern int sys_stat(const char *__restrict__ __file, struct stat *__restrict__ __buf);
extern void sys_exit(int __status);
extern void *sys_malloc(size_t __size);
extern int sys_pthread_join(pthread_t __th, void **__thread_return);
extern int sys_printf(const char *__restrict__ __format, ...);