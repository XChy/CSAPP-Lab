#include "csapp.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

#define MAX_COMMAND_SIZE 16
#define MAX_HOSTNAME_SIZE 64
#define MAX_PORT_SIZE 8
#define MAX_PATH_SIZE 64
#define MAX_VERSION_SIZE 16

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";
static const char *connection_hdr = "Connection: close\r\n";
static const char *proxy_connection_hdr = "Proxy-Connection: close\r\n";

static const char *http_version = "HTTP/1.0";

typedef struct
{
    char command[MAX_COMMAND_SIZE];
    char hostname[MAX_HOSTNAME_SIZE];
    char port[MAX_PORT_SIZE];
    char path[MAX_PATH_SIZE];
    char version[MAX_VERSION_SIZE];
} RequestLine;

typedef struct
{
    char str[MAXLINE];
} RequestHeader;

// Close the file descriptor automatically return -1 if error
int trans(int connfd);
// return -1 if error
int readRequestLine(rio_t *rio, int connfd, char *result);
// return -1 if error
int parseRequestLine(char *buf, RequestLine *result);
// return the number of headers
int readRequestHeader(rio_t *rio, RequestHeader *headers);
// return file descriptor of client
int forwardToServer(RequestLine *line, RequestHeader *header, int headerCount);

int main(int argc, char *argv[])
{
    int listenfd, connfd;
    char hostname[MAXLINE], port[MAXLINE];
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;

    /* Check command line args */
    if (argc != 2)
    {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }

    listenfd = Open_listenfd(argv[1]);

    while (1)
    {
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        Getnameinfo((SA *)&clientaddr, clientlen, hostname, MAXLINE,
                    port, MAXLINE, 0);
        printf("Accepted connection from (%s, %s)\n", hostname, port);

        pthread_t tid;
        Pthread_create(&tid, NULL, trans, connfd);
        Pthread_detach(tid);
    }
    return 0;
}

int trans(int fromClientfd)
{
    rio_t rio;
    RequestLine requestLine;
    char requestStr[MAXLINE];
    RequestHeader headers[20];
    rio_readinitb(&rio, fromClientfd); // init the rio reader
    int headerCount;

    // parse the request
    if (readRequestLine(&rio, fromClientfd, requestStr) < 0)
    {
        return -1;
    }
    if (parseRequestLine(requestStr, &requestLine) < 0)
    {
        return -1;
    }

    printf("Raw request line:%s", requestStr);
    printf("Request line:%s %s:%s\n", requestLine.command, requestLine.hostname, requestLine.port);

    headerCount = readRequestHeader(&rio, headers);

    for (size_t i = 0; i < headerCount; i++)
    {
        printf("Request header:%s", headers[i].str);
    }

    // TODO:Read from cache

    // forward the request to the server,and get data as a client
    printf("Try to connect host:%s port:%s\n", requestLine.hostname, requestLine.port);
    int toServerfd = forwardToServer(&requestLine, headers, headerCount);
    printf("Connected to the server\n");

    Rio_readinitb(&rio, toServerfd); // change rio to server
    char buf[MAXLINE];               // For cache

    int n;
    while (n = rio_readlineb(&rio, buf, MAXLINE)) // Read from server
    {
        Rio_writen(fromClientfd, buf, n); // Write to client
    }

    Close(toServerfd);
    Close(fromClientfd);
    return 0;
}

int readRequestLine(rio_t *rio, int connfd, char *result)
{
    if (!Rio_readlineb(rio, result, MAXLINE))
        return -1;
    return 0;
}

int parseRequestLine(char *str, RequestLine *result)
{
    char command[MAXLINE];
    char url[MAXLINE];
    char version[MAXLINE];
    // The request is illegal , then return -1
    if (sscanf(str, "%s http://%s %s", command, url, version) < 0)
    {
        return -1;
    }

    printf("Valid url:%s\n", url);
    strcpy(result->command, command);
    strcpy(result->version, version);

    // Divide url into (hostname port(optional) path)
    char *p = url;
    while (*p != '\0' && *p != '/' && *p != ':')
    {
        result->hostname[p - url] = *p;
        p++;
    }
    result->hostname[p - url] = '\0';

    // port
    char *portHead = p;
    if (*p == ':')
    {
        p++;
        while (*p != '\0' && *p != '/')
        {
            result->port[p - portHead - 1] = *p;
            p++;
        }
        result->port[p - portHead - 1] = '\0';
    }
    else
    {
        result->port[p - portHead] = '\0';
    }

    if (strlen(result->port) == 0)
    {
        strcpy(result->port, "80");
    }

    // path and query
    char *pathHead = p;
    if (*p == '/')
    {
        while (*p != '\0')
        {
            result->path[p - pathHead] = *p;
            p++;
        }
        result->path[p - pathHead] = '\0';
    }

    return 0;
}

int readRequestHeader(rio_t *rio, RequestHeader *headers)
{
    int i = 0;
    char buf[MAXLINE];
    Rio_readlineb(rio, buf, MAXLINE);
    while (strcmp(buf, "\r\n") != 0)
    {
        strcpy(headers[i].str, buf);
        Rio_readlineb(rio, buf, MAXLINE);
        i++;
    }
    return i;
}

int forwardToServer(RequestLine *line, RequestHeader *header, int headerCount)
{
    int clientfd = Open_clientfd(line->hostname, line->port);
    if (clientfd < 0)
    {
        return -1;
    }
    char buf[MAXLINE];
    int lastPos = 0;
    rio_t rio;

    Rio_readinitb(&rio, clientfd);
    // request
    lastPos += sprintf(buf + lastPos, "%s %s %s\r\n", line->command, line->path, http_version);
    for (size_t i = 0; i < headerCount; i++)
    {
        lastPos += sprintf(buf + lastPos, "%s", header[i].str);
    }
    sprintf(buf + lastPos, "\r\n");
    Rio_writen(clientfd, buf, MAXLINE);

    return clientfd;
}