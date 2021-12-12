#include "wasm_import.h"

int sys_setsockopt(int __fd, int __level, int __optname, const void *__optval, socklen_t __optlen)
{
	return setsockopt(__fd, __level, __optname, __optval, __optlen);
}
int sys_bind(int __fd, const struct sockaddr *__addr, socklen_t __len)
{
	return bind(__fd, __addr, __len);
}
int sys_listen(int __fd, int __n)
{
	return sys_listen(__fd, __n);
}
int sys_accept(int __fd, struct sockaddr *__restrict__ __addr, socklen_t *__restrict__ __addr_len)
{
	return accept(__fd, __addr, __addr_len);
}
int sys_munmap(void *__addr, size_t __len)
{
	return munmap(__addr, __len);
}
void *sys_mmap(void *__addr, size_t __len, int __prot, int __flags, int __fd, off_t __offset)
{
	return mmap(__addr, __len, __prot, __flags, __fd, __offset);
}
int sys_pipe(int *__pipedes)
{
	return pipe(__pipedes);
}
int sys_pthread_create(pthread_t *__restrict__ __newthread, const pthread_attr_t *__restrict__ __attr, void *(*__start_routine)(void *), void *__restrict__ __arg)
{
	return pthread_create(__newthread, __attr, __start_routine, __arg);
}
ssize_t sys_write(int __fd, const void *__buf, size_t __n)
{
	return write(__fd, __buf, __n);
}
ssize_t sys_read(int __fd, void *__buf, size_t __nbytes)
{
	return read(__fd, __buf, __nbytes);
}
int sys_close(int __fd)
{
	return close(__fd);
}
int sys_stat(const char *__restrict__ __file, struct stat *__restrict__ __buf)
{
	return stat(__file, __buf);
}
void sys_exit(int __status)
{
	return exit(__status);
}
void *sys_malloc(size_t __size)
{
	return malloc(__size);
}
int sys_pthread_join(pthread_t __th, void **__thread_return)
{
	return pthread_join(__th, __thread_return);
}
int sys_printf(const char *__restrict__ __format, ...)
{
	return printf("Not Implemented!");
}

void undefined() {
	printf("undefeind!");
	while(1);
}