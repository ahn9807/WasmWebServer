#include "wasm_import.h"
#include <stdarg.h>
#include <errno.h>
struct sockaddr_in serveraddr; /* server's addr */

int sys_socket(int __domain, int __type, int __protocol)
{
	return socket(__domain, __type, __protocol);
}

int sys_open(const char *__file, int __oflag) {
	open(wasm_addr_to_absolute(__file), __oflag);
}

int sys_setsockopt(int __fd, int __level, int __optname, int __optval, socklen_t __optlen)
{
	int temp = __optval;
	return setsockopt(__fd, __level, __optname, &temp, __optlen);
}
int sys_bind(int __fd, int sin_port, int sin_addr)
{
	bzero((char *)&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons((unsigned short)8888);
	return bind(__fd, &serveraddr, sizeof(serveraddr));
}
int sys_listen(int __fd, int __n)
{
	return listen(__fd, __n);
}
int sys_accept(int __fd)
{
	int client_len = sizeof(serveraddr);
	struct sockaddr_in serveraddr; /* server's addr */
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(8888);
	return accept(__fd, &serveraddr, &client_len);
}
int sys_munmap(uintptr_t __addr, size_t __len)
{
	return munmap(__addr, __len);
}
uintptr_t sys_mmap(uintptr_t __addr, size_t __len, int __prot, int __flags, int __fd, off_t __offset)
{
	uintptr_t ret = mmap(__addr, __len, __prot, __flags, __fd, __offset);
	return ret;
}
int sys_pipe(int *__pipedes)
{
	return pipe(wasm_addr_to_absolute(__pipedes));
}
int sys_pthread_create(pthread_t *__restrict__ __newthread, const pthread_attr_t *__restrict__ __attr, void *(*__start_routine)(void *), void *__restrict__ __arg)
{
	return pthread_create(__newthread, __attr, __start_routine, __arg);
}
ssize_t sys_write(int __fd, const uintptr_t __buf, size_t __n)
{
	return write(__fd, wasm_addr_to_absolute(__buf), __n);
}
ssize_t sys_write_fd(int __fd, char *filename_in, size_t len) {
	char* filename = (char *)wasm_addr_to_absolute(filename_in);
	int fd = open(filename, O_RDONLY);
	void* buf = mmap(0, len, PROT_READ, MAP_PRIVATE, fd, 0);
	int ret = write(__fd, buf, len);
	munmap(buf, len);

	return ret;
}

ssize_t sys_read(int __fd, uintptr_t __buf, size_t __nbytes)
{
	int buf = 0;
	int ret = read(__fd, wasm_addr_to_absolute(__buf), __nbytes);
	return ret;
}

int sys_read_pipe(int __fd) {
	int buf = 0;
	int ret = read(__fd, &buf, sizeof(int));

	if (ret != sizeof(int) || buf <= 0) {
		return -1;
	} else {
		return buf;
	}
}

int sys_write_pipe(int __fd, int data) {
	int buf = data;
	int ret = write(__fd, &buf, sizeof(int));

	return ret;
}

int sys_close(int __fd)
{
	if (__fd == 3 || __fd ==4) {
		printf("??????");
	}
	return close(__fd);
}
int sys_stat(const char *__restrict__ __file, struct stat *__restrict__ __buf)
{
	return stat(wasm_addr_to_absolute(__file), wasm_addr_to_absolute(__buf));
}

int sys_filesize(const char *__file) {
	struct stat stat_buf;
	int ret = 0;

	ret = stat(wasm_addr_to_absolute(__file), &stat_buf);

	if (ret < 00 || !stat_buf.st_size) {
		return -1;
	} else {
		return stat_buf.st_size;
	}
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
	va_list list;

	va_start(list, wasm_addr_to_absolute(__format));
	printf(wasm_addr_to_absolute(__format), list);
	va_end(list);

	return 0;
}

void undefined()
{
	printf("undefeind!");
	while (1)
		;
}

void sys_server_error(char *msg)
{
	printf("panic!: %s", wasm_addr_to_absolute(msg));
	exit(-1);
}