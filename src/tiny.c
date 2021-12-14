#include "webserver.c"
#include <pthread.h>

extern char **environ; /* the environment */

#define WORKER_SIZE 8

int main(int argc, char **argv)
{
	int server_sd, worker_sd, pair_sd[2];
	struct server_arguments *args = malloc(sizeof(struct server_arguments));

	/* check command line args */
	if (argc != 2)
	{
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(1);
	}

	if (pipe(pair_sd) < 0)
	{
		server_error("socket bind failed");
		exit(EXIT_FAILURE);
	}

	worker_sd = pair_sd[0];
	server_sd = pair_sd[1];

	printf("server id %d worker id %d\n", server_sd, worker_sd);

	args->portno = argv[1];
	args->message_fd = server_sd;

	pthread_t *threads = malloc((WORKER_SIZE + 1) * sizeof(pthread_t));

	pthread_create(&threads[0], NULL, &server_main, (void *)args);

	for (int i = 0; i < WORKER_SIZE; i++)
	{
		pthread_create(&threads[i + 1], NULL, &server_worker, &worker_sd);
	}

	for (int i = 0; i < WORKER_SIZE + 1; i++)
	{
		pthread_join(threads[i], NULL);
	}
}
