#include <errno.h>
#include "webserver.h"

#define WORKER_SIZE 8

__attribute__((used))
void start_main() {
	int server_sd, worker_sd, pair_sd[2];
	struct server_arguments *args = malloc(sizeof(struct server_arguments));

	/* check command line args */
	if (sys_pipe(pair_sd) < 0)
	{
		sys_server_error("socket bind failed");
		sys_exit(EXIT_FAILURE);
	}

	worker_sd = pair_sd[0];
	server_sd = pair_sd[1];


	args->portno = "8888";
	args->message_fd = server_sd;

	pthread_t *threads = sys_malloc((WORKER_SIZE + 1) * sizeof(pthread_t));

	sys_pthread_create(&threads[0], NULL, server_main, args);

	for (int i = 0; i < WORKER_SIZE; i++)
	{
		sys_pthread_create(&threads[i + 1], NULL, server_worker, &worker_sd);
	}

	for (int i = 0; i < WORKER_SIZE + 1; i++)
	{
		sys_pthread_join(threads[i], NULL);
	}
}

__attribute__((used))
void *server_main(void *args_in)
{
	struct msghdr parent_msg;
	struct server_arguments *args = args_in;

	struct sockaddr_in serveraddr; /* server's addr */
	struct sockaddr_in clientaddr; /* client addr */
	int clientlen = sizeof(clientaddr);
	int parentfd = sys_socket(AF_INET, SOCK_STREAM, 0);
	int optval = 1;


	int portno = atoi(args->portno);

	if (parentfd < 0)
		sys_server_error("ERROR opening socket");

	/* allows us to restart server immediately */
	sys_setsockopt(parentfd, SOL_SOCKET, SO_REUSEADDR,
			   1, sizeof(int));

	/* sys_bind port to socket */
	bzero((char *)&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons((unsigned short)portno);
	if (sys_bind(parentfd, (struct sockaddr *)&serveraddr,
			 sizeof(serveraddr)) < 0)
		sys_server_error("ERROR on sys_binding");

	/* get us sys_ready to accept connection requests */
	if (sys_listen(parentfd, 5) < 0) /* allow 5 requests to queue up */
		sys_server_error("ERROR on listen");

	while (1)
	{
		/* wait for a connection request */
		// int childfd = sys_accept(parentfd, (struct sockaddr *)&clientaddr, (socklen_t *)&clientlen);
		int childfd = sys_accept(parentfd);
		if (childfd < 0)
			sys_server_error("ERROR on accept");

		if (sys_write_pipe(args->message_fd, childfd) != sizeof(int)) {
			sys_server_error("send failed");
		}
	}
}

__attribute__((used))
void *server_worker(void *message_fd_in)
{
	FILE *stream; /* stream version of childfd */
	int childfd;
	int is_static = 0;
	int filesize = 0;
	int fd;			  /* static content filedes */
	uint64_t p;		  /* temporary pointer */
	int message_fd = *(int *)message_fd_in;

	/* variables for connection I/O */
	char *buf;
	char *method;
	char *uri;
	char *version;
	char *filename;
	char *filetype;

	int rc;


	while (1)
	{
		buf = malloc(sizeof(char) * BUFSIZE);
		method = malloc(sizeof(char) * BUFSIZE);
		uri = malloc(sizeof(char) * BUFSIZE);
		version = malloc(sizeof(char) * BUFSIZE);
		filename = malloc(sizeof(char) * BUFSIZE);
		filetype = malloc(sizeof(char) * BUFSIZE);

		childfd = sys_read_pipe(message_fd);
		if(childfd == -1) {
			sys_server_error("ak...");
			sys_exit(-1);
		}


		/* get the HTTP request line */
		sys_read(childfd, buf, BUFSIZE);
		sscanf(buf, "%s %s %s\n", method, uri, version);

		/* tiny only supports the GET method */
		if (strcasecmp(method, "GET"))
		{
			sys_write(childfd, "HTTP/1.1 501 Not Implemented", sizeof("HTTP/1.1 501 Not Implemented"));
			sys_close(childfd);
			sys_exit(-1);
		}

		if (!strstr(uri, "cgi-bin"))
		{ /* static content */
			is_static = 1;
			strcpy(filename, ".");
			strcat(filename, uri);
			if (uri[strlen(uri) - 1] == '/')
				strcat(filename, "index.html");

			filesize = sys_filesize(filename);

			if (filesize < 0)
			{
				sys_write(childfd, "HTTP/1.1 404 Not Found", sizeof("HTTP/1.1 404 Not Found"));
				sys_close(childfd);
				continue;
			}
		}

		/* serve static content */
		if (is_static)
		{
			if (strstr(filename, ".html"))
				strcpy(filetype, "text/html");
			else if (strstr(filename, ".gif"))
				strcpy(filetype, "image/gif");
			else if (strstr(filename, ".jpg"))
				strcpy(filetype, "image/jpg");
			else
				strcpy(filetype, "text/plain");

			/* print response header */
			sys_write(childfd, "HTTP/1.1 200 OK\r\n\r\n", sizeof("HTTP/1.1 200 OK\r\n\r\n") - 1);

			/* Use mmap to return arbitrary-sized response body */
			// fd = sys_open(filename, O_RDONLY);
			// p = sys_mmap(0, filesize, PROT_READ, MAP_PRIVATE, fd, 0);
			sys_write_fd(childfd, filename, filesize);
			// sys_munmap(p, filesize);
		}
		else
		{
			sys_write(childfd, "HTTP/1.1 501 Not Implemented", sizeof("HTTP/1.1 501 Not Implemented"));
			sys_server_error("dynamic web page is not supported");
			sys_exit(-1);
		}

		/* clean up */
		free(buf);
		free(method);
		free(uri);
		free(version);
		free(filename);
		free(filetype);
		sys_close(childfd);
	}
}
