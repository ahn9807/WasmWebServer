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
#include <pthread.h>

#define BUFSIZE 1024

struct server_arguments {
	int message_fd;
	char *portno;
};

__attribute__((used))
void * server_main(void *);
__attribute__((used))
void * server_worker(void *);

extern int sys_socket(int __domain, int __type, int __protocol);
extern void sys_server_error(char *msg);
extern int sys_setsockopt(int __fd, int __level, int __optname, int __optval, socklen_t __optlen);
extern int sys_bind(int __fd, int sin_port, int sin_addr);
extern int sys_listen(int __fd, int __n);
extern int sys_accept(int __fd);
extern int sys_munmap(uint64_t __addr, size_t __len);
extern uint64_t sys_mmap(uint64_t __addr, size_t __len, int __prot, int __flags, int __fd, off_t __offset);
extern int sys_pipe(int *__pipedes);
extern int sys_pthread_create(pthread_t *__restrict__ __newthread, const pthread_attr_t *__restrict__ __attr, void *(*__start_routine)(void *), void *__restrict__ __arg);
extern ssize_t sys_write(int __fd, uint64_t __buf, size_t __n);
extern ssize_t sys_read(int __fd, uint64_t __buf, size_t __nbytes);
extern int sys_close(int __fd);
extern int sys_stat(const char *__restrict__ __file, struct stat *__restrict__ __buf);
extern void sys_exit(int __status);
extern void *sys_malloc(size_t __size);
extern int sys_pthread_join(pthread_t __th, void **__thread_return);
extern int sys_printf(const char *__restrict__ __format, ...);
extern int sys_open(const char *__file, int __oflag);
extern int sys_filesize(const char *__file);
extern ssize_t sys_write_fd(int __fd, char *filename_in, size_t len);
extern int sys_write_pipe(int __fd, int data);
extern int sys_read_pipe(int __fd);