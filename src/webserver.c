#include <errno.h>

#include "webserver.h"

#define SO_REUSEADDR 2

void *server_main(void *args_in)
{
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

void handle_resp(int childfd) {
    struct stat sbuf; /* file status */
    char *buf = malloc(1024);
    char *method = malloc(BUFSIZE);
    char *uri = malloc(BUFSIZE);
    char *version = malloc(BUFSIZE);
    char *filename = malloc(BUFSIZE);

    /* get the HTTP request line */
    read(childfd, buf, BUFSIZE);
    sscanf(buf, "%s %s %s\n", method, uri, version);
    if (strlen(uri) == 1 && uri[0] == '/') {
        strcpy(filename, "./index.html");
    } else {
        filename[0] = '.';
        strcpy(&filename[1], uri);
    }

    int fd;
    if ((fd = open(filename, O_RDONLY)) < 0) {
        write(childfd, "HTTP/1.1 404 Not Found", sizeof("HTTP/1.1 404 Not Found"));
    } else {
        if (fstat(fd, &sbuf) < 0) {
            perror("stat()");
        } else {
            write(childfd, "HTTP/1.1 200 OK\r\n\r\n", sizeof("HTTP/1.1 200 OK\r\n\r\n") - 1);
            char *b = (char *) malloc(sbuf.st_size);
            if (read(fd, b, sbuf.st_size) != sbuf.st_size) {
                perror("read()");
            } else {
                write(childfd, b, sbuf.st_size);
            }
            free(b);
            free(buf);
            free(method);
            free(uri);
            free(version);
            free(filename);
        }
    }
    close(childfd);
}

void *server_worker(void *message_fd_in)
{
    int childfd;
    int message_fd = *(int *)message_fd_in;

    while (1) {
        if (read(message_fd, &childfd, sizeof(childfd)) != sizeof(childfd)) {
            perror("recvmsg() failed");
            close(message_fd);
            exit(-1);
        } else if (childfd <= 0) {
            printf("Descriptor was not received\n");
            close(message_fd);
            exit(-1);
        }
        handle_resp(childfd);
    }
}
