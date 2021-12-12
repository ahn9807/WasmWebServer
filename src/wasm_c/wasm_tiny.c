#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define WASM_RT_MODULE_PREFIX tiny_

#include "wasm_server.h"
#include "wasm_import.c"

#define WORKER_SIZE 8

extern u32 (*tiny_Z_mallocZ_ii)(u32);

/* import: 'env' 'sys_setsockopt' */
u32 (*Z_envZ_sys_setsockoptZ_iiiiii)(u32, u32, u32, u32, u32);
/* import: 'env' 'sys_bind' */
u32 (*Z_envZ_sys_bindZ_iiii)(u32, u32, u32);
/* import: 'env' 'sys_listen' */
u32 (*Z_envZ_sys_listenZ_iii)(u32, u32);
/* import: 'env' 'sys_accept' */
u32 (*Z_envZ_sys_acceptZ_iiii)(u32, u32, u32);
/* import: 'env' 'sys_write' */
u32 (*Z_envZ_sys_writeZ_iiii)(u32, u32, u32);
/* import: 'env' 'sys_read' */
u32 (*Z_envZ_sys_readZ_iiii)(u32, u32, u32);
/* import: 'env' 'sys_close' */
u32 (*Z_envZ_sys_closeZ_ii)(u32);
/* import: 'env' 'sys_exit' */
void (*Z_envZ_sys_exitZ_vi)(u32);
/* import: 'env' 'sys_stat' */
u32 (*Z_envZ_sys_statZ_iii)(u32, u32);
/* import: 'env' 'sys_mmap' */
u32 (*Z_envZ_sys_mmapZ_iiiiiij)(u32, u32, u32, u32, u32, u64);
/* import: 'env' 'sys_munmap' */
u32 (*Z_envZ_sys_munmapZ_iii)(u32, u32);
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

void* wasm_addr_to_absolute(u32 wasm_addr) {
  return &tiny_Z_memory->data[wasm_addr];
}

void write_u32(u32 wasm_addr, u32 value) {
  *(u32*)wasm_addr_to_absolute(wasm_addr) = value;
}

u32 read_u32(u32 wasm_addr) {
  return *(u32*)wasm_addr_to_absolute(wasm_addr);
}

int main(int argc, char **argv)
{
	tiny_init();

	Z_envZ___syscall_socketZ_iiiiiii = &sys_setsockopt;
	Z_envZ_sys_acceptZ_iiii = &sys_accept;
	Z_envZ_sys_bindZ_iiii = &sys_bind;
	Z_envZ_sys_closeZ_ii = &sys_close;
	Z_envZ_sys_exitZ_vi = &sys_exit;
	Z_envZ_sys_listenZ_iii = &sys_listen;
	Z_envZ_sys_mallocZ_ii = &sys_malloc;
	Z_envZ_sys_mmapZ_iiiiiij = &sys_mmap;
	Z_envZ_sys_munmapZ_iii = &sys_munmap;
	Z_envZ_sys_pipeZ_ii = &sys_pipe;
	Z_envZ_sys_printfZ_iii = &sys_printf;
	Z_envZ_sys_pthread_createZ_iiiii = &sys_pthread_create;
	Z_envZ_sys_pthread_joinZ_iii = &sys_pthread_join;
	Z_envZ_sys_readZ_iiii = &sys_read;
	Z_envZ_sys_statZ_iii = &sys_stat;
	Z_envZ_sys_writeZ_iiii = &sys_write;
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
		sys_printf("usage: ./server <port>\n");
		sys_exit(1);
	}

	if (sys_pipe(pair_sd) < 0)
	{
		server_error("socket bind failed");
		sys_exit(EXIT_FAILURE);
	}

	worker_sd = pair_sd[0];
	server_sd = pair_sd[1];

	sys_printf("server id %d worker id %d\n", server_sd, worker_sd);

	args->portno = argv[1];
	args->message_fd = server_sd;

	pthread_t *threads = malloc((WORKER_SIZE + 1) * sizeof(pthread_t));


	u32 wasm_server = w2c_malloc(sizeof(args));
	memcpy(wasm_addr_to_absolute(wasm_server), args, sizeof(struct server_arguments));

	u32 wasm_worker = w2c_malloc(sizeof(worker_sd));
	memcpy(wasm_addr_to_absolute(wasm_worker), &worker_sd, sizeof(worker_sd));


	// sys_pthread_create(&threads[0], NULL, tiny_Z_server_mainZ_ii, wasm_server);

	for (int i = 0; i < WORKER_SIZE; i++)
	{
		sys_pthread_create(&threads[i + 1], NULL, tiny_Z_server_workerZ_ii, wasm_worker);
	}

	// for (int i = 0; i < WORKER_SIZE + 1; i++)
	// {
	// 	sys_pthread_join(threads[i], NULL);
	// }

	tiny_Z_server_mainZ_ii(wasm_server);

	while(1);

	return 0;
}