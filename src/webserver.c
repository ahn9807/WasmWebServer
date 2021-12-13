#include <errno.h>

#include "webserver.h"

#define SO_REUSEADDR 2


void *server_main(void *args_in)
{
	struct msghdr parent_msg;
	struct server_arguments *args = args_in;

	struct sockaddr_in serveraddr; /* server's addr */
	struct sockaddr_in clientaddr; /* client addr */
	int clientlen = sizeof(clientaddr);
	int parentfd = socket(AF_INET, SOCK_STREAM, 0);
	int optval = 1;

	int portno = atoi(args->portno);

	if (parentfd < 0)
		server_error("ERROR opening socket");

	/* allows us to restart server immediately */
	setsockopt(parentfd, SOL_SOCKET, SO_REUSEADDR,
			   (const void *)&optval, sizeof(int));

	/* bind port to socket */
	bzero((char *)&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons((unsigned short)portno);
	if (bind(parentfd, (struct sockaddr *)&serveraddr,
			 sizeof(serveraddr)) < 0)
		server_error("ERROR on binding");

	/* get us ready to accept connection requests */
	if (listen(parentfd, 5) < 0) /* allow 5 requests to queue up */
		server_error("ERROR on listen");

	while (1)
	{
		/* wait for a connection request */
		int childfd = accept(parentfd, (struct sockaddr *)&clientaddr, (socklen_t *)&clientlen);
		if (childfd < 0)
			server_error("ERROR on accept");

		if (write(args->message_fd, &childfd, sizeof(childfd)) < 0)
		{
			server_error("send failed");
		}
	}
}

void *server_worker(void *message_fd_in)
{
	FILE *stream; /* stream version of childfd */
	int childfd;
	int is_static = 0;
	struct stat sbuf; /* file status */
	int fd;			  /* static content filedes */
	char *p;		  /* temporary pointer */
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

		memset(&childfd, 0, sizeof(childfd));
		rc = read(message_fd, &childfd, sizeof(childfd));
		if (rc != sizeof(childfd))
		{
			perror("recvmsg() failed");
			close(message_fd);
			exit(-1);
		}
		else if (childfd <= 0)
		{
			printf("Descriptor was not received\n");
			close(message_fd);
			exit(-1);
		}

		/* get the HTTP request line */
		read(childfd, buf, BUFSIZE);
		sscanf(buf, "%s %s %s\n", method, uri, version);

		/* tiny only supports the GET method */
		if (strcasecmp(method, "GET"))
		{
			write(childfd, "HTTP/1.1 501 Not Implemented", sizeof("HTTP/1.1 501 Not Implemented"));
			close(childfd);
			continue;
		}

		if (!strstr(uri, "cgi-bin"))
		{ /* static content */
			is_static = 1;
			strcpy(filename, ".");
			strcat(filename, uri);
			if (uri[strlen(uri) - 1] == '/')
				strcat(filename, "index.html");

			if (stat(filename, &sbuf) < 0)
			{
				write(childfd, "HTTP/1.1 404 Not Found", sizeof("HTTP/1.1 404 Not Found"));
				close(childfd);
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
			write(childfd, "HTTP/1.1 200 OK\r\n\r\n", sizeof("HTTP/1.1 200 OK\r\n\r\n")-1);

			/* Use mmap to return arbitrary-sized response body */
			fd = open(filename, O_RDONLY);
			p = mmap(0, sbuf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
			write(childfd, p, sbuf.st_size);
			munmap(p, sbuf.st_size);
		}
		else
		{
			write(childfd, "HTTP/1.1 501 Not Implemented", sizeof("HTTP/1.1 501 Not Implemented"));
			server_error("dynamic web page is not supported");
			exit(-1);
		}

		free(buf);
		free(method);
		free(uri);
		free(version);
		free(filename);
		free(filetype);

		/* clean up */
		close(childfd);
	}
}
