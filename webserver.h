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
	perror(msg);
	exit(1);
}

void * server_main(void *);
void * server_worker(void *);