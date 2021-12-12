#include <error.h>
#include <errno.h>

#include "webserver.h"

void cerror(FILE *stream, char *cause, char *errno_in,
			char *shortmsg, char *longmsg)
{
	fprintf(stream, "HTTP/1.1 %s %s\n", errno_in, shortmsg);
	fprintf(stream, "Content-type: text/html\n");
	fprintf(stream, "\n");
	fprintf(stream, "<html><title>Tiny Error</title>");
	fprintf(stream, "<body bgcolor="
					"ffffff"
					">\n");
	fprintf(stream, "%s: %s\n", errno_in, shortmsg);
	fprintf(stream, "<p>%s: %s\n", longmsg, cause);
	fprintf(stream, "<hr><em>The Tiny Web server</em>\n");
}

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
		int childfd = accept(parentfd, (struct sockaddr *)&clientaddr, &clientlen);
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
	char buf[BUFSIZE];		/* message buffer */
	char method[BUFSIZE];	/* request method */
	char uri[BUFSIZE];		/* request uri */
	char version[BUFSIZE];	/* request method */
	char filename[BUFSIZE]; /* path derived from uri */
	char filetype[BUFSIZE]; /* path derived from uri */
	char cgiargs[BUFSIZE];	/* cgi argument list */

	struct iovec iov[1];
	struct msghdr child_msg;
	char msg_buffer[80];
	int rc;

	while (1)
	{
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
			cerror(stream, method, "501", "Not Implemented",
				   "Tiny does not implement this method");
			fclose(stream);
			close(childfd);
			continue;
		}

		/* read (and ignore) the HTTP headers */
		// fgets(buf, BUFSIZE, stream);
		// while (strcmp(buf, "\r\n"))
		// {
		// 	fgets(buf, BUFSIZE, stream);
		// }

		/* parse the uri [crufty] */
		if (!strstr(uri, "cgi-bin"))
		{ /* static content */
			is_static = 1;
			strcpy(cgiargs, "");
			strcpy(filename, ".");
			strcat(filename, uri);
			if (uri[strlen(uri) - 1] == '/')
				strcat(filename, "index.html");

			if (stat(filename, &sbuf) < 0)
			{
				// cerror(stream, filename, "404", "Not found",
					//    "Tiny couldn't find this file");
				// fclose(stream);
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
			write(childfd, "HTTP/1.1 200 OK\r\n\r\n", sizeof("HTTP/1.1 200 OK\r\n\r\n"));
			// fprintf(stream, "HTTP/1.1 200 OK\n");
			// fprintf(stream, "Server: Tiny Web Server\n");
			// fprintf(stream, "Content-length: %d\n", (int)sbuf.st_size);
			// fprintf(stream, "Content-type: %s\n", filetype);
			// fprintf(stream, "\r\n");
			// fflush(stream);

			/* Use mmap to return arbitrary-sized response body */
			fd = open(filename, O_RDONLY);
			p = mmap(0, sbuf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
			write(childfd, p, sbuf.st_size);
			munmap(p, sbuf.st_size);
		}
		else
		{
			server_error("dynamic web page is not supported");
			exit(-1);
		}

		/* clean up */
		// fclose(stream);
		close(childfd);
	}
}