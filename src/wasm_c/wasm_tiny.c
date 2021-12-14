#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define WASM_RT_MODULE_PREFIX tiny_

#include "wasm_server.h"
#include "wasm_import.c"

#define WORKER_SIZE 1

extern u32 (*tiny_Z_mallocZ_ii)(u32);

u32 (*Z_envZ_sys_filesizeZ_ii)(u32);
/* import: 'env' 'sys_socket' */
u32 (*Z_envZ_sys_socketZ_iiii)(u32, u32, u32);
/* import: 'env' 'sys_setsockopt' */
u32 (*Z_envZ_sys_setsockoptZ_iiiiii)(u32, u32, u32, u32, u32);
/* import: 'env' 'sys_setsockopt' */
u32 (*Z_envZ_sys_setsockoptZ_iiiiii)(u32, u32, u32, u32, u32);
/* import: 'env' 'sys_bind' */
u32 (*Z_envZ_sys_bindZ_iiii)(u32, u32, u32);
/* import: 'env' 'sys_listen' */
u32 (*Z_envZ_sys_listenZ_iii)(u32, u32);
/* import: 'env' 'sys_accept' */
u32 (*Z_envZ_sys_acceptZ_ii)(u32);
u32 (*Z_envZ_sys_openZ_iii)(u32, u32);
/* import: 'env' 'sys_write' */
u32 (*Z_envZ_sys_writeZ_iiji)(u32, u64, u32);
/* import: 'env' 'sys_read' */
u32 (*Z_envZ_sys_readZ_iiji)(u32, u64, u32);
/* import: 'env' 'sys_close' */
u32 (*Z_envZ_sys_closeZ_ii)(u32);
/* import: 'env' 'sys_exit' */
void (*Z_envZ_sys_exitZ_vi)(u32);
/* import: 'env' 'sys_stat' */
u32 (*Z_envZ_sys_statZ_iii)(u32, u32);
/* import: 'env' 'sys_mmap' */
u64 (*Z_envZ_sys_mmapZ_jjiiiij)(u64, u32, u32, u32, u32, u64);
/* import: 'env' 'sys_munmap' */
u32 (*Z_envZ_sys_munmapZ_iji)(u64, u32);
/* import: 'env' 'sys_malloc' */
u32 (*Z_envZ_sys_mallocZ_ii)(u32);
/* import: 'env' 'sys_printf' */
u32 (*Z_envZ_sys_printfZ_iii)(u32, u32);
/* import: 'env' 'sys_pipe' */
u32 (*Z_envZ_sys_pipeZ_ii)(u32);
/* import: 'env' 'sys_pthread_create' */
u32 (*Z_envZ_sys_pthread_createZ_iiiii)(u32, u32, u32, u32);
/* import: 'env' 'sys_pthread_join' */
u32 (*Z_envZ_sys_pthread_joinZ_iii)(u32, u32);
/* import: 'wasi_snapshot_preview1' 'fd_close' */
u32 (*Z_wasi_snapshot_preview1Z_fd_closeZ_ii)(u32);
/* import: 'wasi_snapshot_preview1' 'fd_seek' */
u32 (*Z_wasi_snapshot_preview1Z_fd_seekZ_iijii)(u32, u64, u32, u32);
/* import: 'wasi_snapshot_preview1' 'fd_write' */
u32 (*Z_wasi_snapshot_preview1Z_fd_writeZ_iiiii)(u32, u32, u32, u32);
/* import: 'wasi_snapshot_preview1' 'proc_exit' */
void (*Z_wasi_snapshot_preview1Z_proc_exitZ_vi)(u32);
/* import: 'wasi_snapshot_preview1' 'args_sizes_get' */
u32 (*Z_wasi_snapshot_preview1Z_args_sizes_getZ_iii)(u32, u32);
/* import: 'wasi_snapshot_preview1' 'args_get' */
u32 (*Z_wasi_snapshot_preview1Z_args_getZ_iii)(u32, u32);
/* import: 'env' '__syscall_socket' */
u32 (*Z_envZ___syscall_socketZ_iiiiiii)(u32, u32, u32, u32, u32, u32);
/* import: 'env' 'sys_server_error' */
void (*Z_envZ_sys_server_errorZ_vi)(u32);
u32 (*Z_envZ_sys_write_fdZ_iiii)(u32, u32, u32);
u32 (*Z_envZ_sys_read_pipeZ_ii)(u32);
u32 (*Z_envZ_sys_write_pipeZ_iii)(u32, u32);
u32 (*Z_envZ_sys_write_directZ_iiji)(u32, u64, u32);

int main(int argc, char **argv)
{
	tiny_init();

	Z_envZ_sys_socketZ_iiii = &sys_socket;
	Z_envZ_sys_setsockoptZ_iiiiii = &sys_setsockopt;
	Z_envZ_sys_acceptZ_ii = &sys_accept;
	Z_envZ_sys_bindZ_iiii = &sys_bind;
	Z_envZ_sys_closeZ_ii = &sys_close;
	Z_envZ_sys_exitZ_vi = &sys_exit;
	Z_envZ_sys_listenZ_iii = &sys_listen;
	Z_envZ_sys_mallocZ_ii = &sys_malloc;
	Z_envZ_sys_mmapZ_jjiiiij = &sys_mmap;
	Z_envZ_sys_munmapZ_iji = &sys_munmap;
	Z_envZ_sys_pipeZ_ii = &sys_pipe;
	Z_envZ_sys_printfZ_iii = &sys_printf;
	Z_envZ_sys_pthread_createZ_iiiii = &sys_pthread_create;
	Z_envZ_sys_pthread_joinZ_iii = &sys_pthread_join;
	Z_envZ_sys_write_directZ_iiji = &sys_write_direct;
	Z_envZ_sys_openZ_iii = &sys_open;
	Z_envZ_sys_readZ_iiji = &sys_read;
	Z_envZ_sys_statZ_iii = &sys_stat;
	Z_envZ_sys_filesizeZ_ii = &sys_filesize;
	Z_envZ_sys_writeZ_iiji = &sys_write;
	Z_envZ_sys_write_fdZ_iiii = &sys_write_fd;
	Z_envZ_sys_server_errorZ_vi = &sys_server_error;
	Z_envZ_sys_read_pipeZ_ii = &sys_read_pipe;
	Z_envZ_sys_write_pipeZ_iii = &sys_write_pipe;
	Z_wasi_snapshot_preview1Z_args_getZ_iii = &undefined;
	Z_wasi_snapshot_preview1Z_args_sizes_getZ_iii = &undefined;
	Z_wasi_snapshot_preview1Z_fd_closeZ_ii = &undefined;
	Z_wasi_snapshot_preview1Z_fd_seekZ_iijii = &undefined;
	Z_wasi_snapshot_preview1Z_fd_writeZ_iiiii = &undefined;
	Z_wasi_snapshot_preview1Z_proc_exitZ_vi = &undefined;

	printf("start server\n");

	int server_sd, worker_sd, pair_sd[2];
	struct server_arguments *args = sys_malloc(sizeof(struct server_arguments));

	/* check command line args */
	if (argc != 2)
	{
		printf("usage: ./server <port>\n");
		sys_exit(1);
	}

	if (pipe(pair_sd) < 0)
	{
		error("socket bind failed");
	}

	worker_sd = pair_sd[0];
	server_sd = pair_sd[1];

	printf("server id %d worker id %d\n", server_sd, worker_sd);
	printf("server id %d worker id %d\n", server_sd, worker_sd);

	args->portno = argv[1];
	args->message_fd = server_sd;

	pthread_t *threads = malloc((WORKER_SIZE + 1) * sizeof(pthread_t));

	u32 wasm_server = w2c_malloc(sizeof(args));
	memcpy(wasm_addr_to_absolute(wasm_server), args, sizeof(struct server_arguments));

	u32 wasm_worker = w2c_malloc(sizeof(worker_sd));
	memcpy(wasm_addr_to_absolute(wasm_worker), &worker_sd, sizeof(worker_sd));

	sys_pthread_create(&threads[0], NULL, tiny_Z_server_mainZ_ii, wasm_server);

	for (int i = 0; i < WORKER_SIZE; i++)
	{
		sys_pthread_create(&threads[i + 1], NULL, tiny_Z_server_workerZ_ii, wasm_worker);
	}

	for (int i = 0; i < WORKER_SIZE + 1; i++)
	{
		sys_pthread_join(threads[i], NULL);
	}
}