#include <stdio.h>

#include "csapp.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400
#define CACHE_OBJS_COUNT 10

/* You won't lose style points for including this long line in your code */



void client_error(int fd, char *cause, char *errnum,
                  char *shortmsg, char *longmsg);

void build_new_header(char *http_header, char *hostname, char *path, int port, rio_t *client_rio);

void parse_uri(char *uri, char *hostname, char *path, int *port);

void *thread(void *vargp);

void doit(int connfd);

void cache_init();

void cache_uri(char *uri, char *buf);

void cache_LRU(int index);

void cache_uri(char *uri, char *buf);

void readerPre(int i);

void readerAfter(int i);

void writePre(int i);

void writeAfter(int i);

typedef struct {
    char cache_obj[MAX_OBJECT_SIZE];
    char cache_url[MAXLINE];
    int LRU;
    int isEmpty;

    int readCount;           /*count of readers*/
    sem_t wmutex;           /*protects accesses to write*/
    sem_t rmutex;           /*protects accesses to read*/
    sem_t ctMutex;          /*protects accesses to count*/

} cache_block;

typedef struct {
    cache_block cacheobjs[CACHE_OBJS_COUNT];  /*ten cache blocks*/

} Cache;

Cache cache;
int LRU_MAGIC_NUMBER = 1;

int main(int argc, char **argv) {
    int listenfd, *connfd, port, clientlen;
    struct sockaddr_in clientaddr;
    char ClientHostname[MAXLINE], ClientPort[MAXLINE];

    pthread_t tid;

    Signal(SIGPIPE, SIG_IGN);//need to improve

    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }

    cache_init();

    port = atoi(argv[1]); /* the server listens on a port passed on the command line */
    listenfd = Open_listenfd(argv[1]);
    while (1) {
        clientlen = sizeof(clientaddr);
        connfd = Malloc(sizeof(int));  //avoid Synchronization problem
        *connfd = Accept(listenfd, (SA *) &clientaddr, &clientlen);
        Getnameinfo((SA *) &clientaddr, clientlen, ClientHostname, MAXLINE, ClientPort, MAXLINE, 0);
        printf("Accepted connection from (%s, %s)\n", ClientHostname, ClientPort);
        Pthread_create(&tid, NULL, thread, connfd); //处理函数，这个是create了一个新线程
    }


}

void *thread(void *vargp) {
    int connfd = *((int *) vargp);
    Pthread_detach(pthread_self()); //将线程分离，让这个线程计数结束后自己回收资源
    Free(vargp);
    doit(connfd);
    return NULL;
}

void doit(int connfd) {
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char new_http_header[MAXLINE];
    /*store the request line arguments*/
    char hostname[MAXLINE], path[MAXLINE];
    int port, serverfd, n;
    char portToStr[100];
//    char url_store[100];
    rio_t rio, server_rio;/*rio is client's rio,server_rio is endserver's rio*/

    Rio_readinitb(&rio, connfd);
    Rio_readlineb(&rio, buf, MAXLINE);
    if (sscanf(buf, "%s %s %s", method, uri, version) < 3) {
        fprintf(stderr, "bad request, sscanf error");
        fprintf(stderr, "req line = %s\n", buf);
        client_error(fd, method, "400", "Bad Request", "Sscanf Error");
        close_fd(fd, 0);
        return;
    }

    if (strcasecmp(method, "GET")) {
        fprintf(stderr, "bad request, not implemented");
        fprintf(stderr, "req line = %s\n", buf);
        client_error(connfd, method, "501", "Bad Request", "Not Implemented");

        return;
    }


    if (cache_find(url))/*in cache then return the cache content*/
        return;


    parse_uri(uri, hostname, path, &port);

    /*build the http header which will send to the end server*/
    build_new_header(new_http_header, hostname, path, &rio);


    //Connect to the server
    sprintf(portToStr, "%d", port);

    if ((serverfd = Open_clientfd(hostname, portToStr)) < 0) {
        fprintf(stderr, "connection failed\n");
        client_error(connfd, method, "404", "Bad Connection", "Connection Failed");
        return;
    }

    //Send request to the server
    Rio_readinitb(&server_rio, serverfd);

    Rio_writen(serverfd, new_http_header, MAXLINE);


    char cachebuf[MAX_OBJECT_SIZE];
    int sizebuf = 0;

    //Read from the server and send to the client
    while ((n = Rio_readlineb(&server_rio, buf, MAXLINE)) != 0) {
        printf("proxy received %d bytes,then send\n", n);
        if (sizebuf < MAX_OBJECT_SIZE) strcat(cachebuf, buf);
        Rio_writen(connfd, buf, n);
    }

    Close(serverfd);
    Close(connfd);

    if (sizebuf < MAX_OBJECT_SIZE) {
        cache_uri(url, cachebuf);
    }
}

void build_new_header(char *http_header, char *hostname, char *path, rio_t *client_rio) {
    char host_hdr[MAXLINE], buf[MAXLINE], other_hdr[MAXLINE];
    while (Rio_readlineb(client_rio, buf, MAXLINE) > 0) {
        if (strcmp(buf, "\r\n") == 0) break;/*EOF*/

        if (!strncasecmp(buf, "Host", strlen("Host")))/*Host:*/
        {
            strcpy(host_hdr, buf);
            continue;
        }

        if (!strncasecmp(buf, "Connection", strlen("Connection"))
            && !strncasecmp(buf, "Proxy-Connection", strlen("Proxy-Connection"))
            && !strncasecmp(buf, "User-Agent", strlen("User-Agent"))) {
            strcat(other_hdr, buf);
        }
    }

    if (strlen(host_hdr) == NULL) {
        sprintf(host_hdr, "Host: %s\r\n", hostname);
    }

    sprintf(http_header,
            "GET %s HTTP/1.0\r\n" / //  request line
            //request head
            "%s" /  //the host
            "Connection: close\r\n" / // Connection
            "Proxy-Connection: close\r\n" / // Proxy-Connection
            "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n" / //User-Agent
            "%s" /
            "\r\n",
            path,
            host_hdr,
            other_hdr
    );

}

void parse_uri(char *uri, char *hostname, char *path, int *port, int connfd) {
    if (strstr(uri, "https://") == uri) {
        fprintf(stderr, "Error: invalid uri!\n");
        client_error(connfd, method, "400", "Bad Request", "invalid uri!");

        close_fd(connfd, 0);
        exit(0);
    }

    *port = 80;
    char *pos = strstr(uri, "//");

    pos = pos != NULL ? pos + 2 : uri;

    char *pos2 = strstr(pos, ":");
    if (pos2 != NULL) {
        *pos2 = '\0';
        sscanf(pos, "%s", hostname);
        sscanf(pos2 + 1, "%d%s", port, path);
    } else {
        pos2 = strstr(pos, "/");
        if (pos2 != NULL) {
            *pos2 = '\0';
            sscanf(pos, "%s", hostname);
            *pos2 = '/';
            sscanf(pos2, "%s", path);
        } else
            sscanf(pos, "%s", hostname);

    }
    return;
}

void client_error(int fd, char *cause, char *errnum,
                  char *shortmsg, char *longmsg) {
    char buf[MAXLINE], body[MAXBUF];

    /* Build the HTTP response body */
    sprintf(body, "%s: %s\r\n", errnum, shortmsg);
    sprintf(body, "%s%s: %s", body, longmsg, cause);

    /* Print the HTTP response */
    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: text/html\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-length: %d\r\n\r\n", (int) strlen(body));
    Rio_writen(fd, buf, strlen(buf));
    Rio_writen(fd, body, strlen(body));
}

void cache_init() {
    int i;
    for (i = 0; i < CACHE_OBJS_COUNT; i++) {
        cache.cacheobjs[i].LRU = 0;
        cache.cacheobjs[i].isEmpty = 1;
        cache.cacheobjs[i].readCount = 0;
        Sem_init(&cache.cacheobjs[i].wmutex, 0, 1);
        Sem_init(&cache.cacheobjs[i].rmutex, 0, 1);
        Sem_init(&cache.cacheobjs[i].cmutex, 0, 1);

    }
}

/*find url is in the cache or not */
int cache_find(char *url) {
    int i;
    for (i = 0; i < CACHE_OBJS_COUNT; i++) {
        readerPre(i);
        if ((cache.cacheobjs[i].isEmpty == 0) && (strcmp(url, cache.cacheobjs[i].cache_url) == 0)) {
            Rio_writen(connfd, cache.cacheobjs[i].cache_obj, strlen(cache.cacheobjs[i].cache_obj));
            readerAfter(i);

            writePre(i);
            cache.cacheobjs[i].LRU = LRU_MAGIC_NUMBER++;
            writeAfter(i);

            return 1;
        };
        readerAfter(i);

    }
    return 0; /*can not find url in the cache*/

}


void cache_uri(char *uri, char *buf) {


    int i = cache_eviction();

    writePre(i);/*writer P*/

    strcpy(cache.cacheobjs[i].cache_obj, buf);
    strcpy(cache.cacheobjs[i].cache_url, uri);
    cache.cacheobjs[i].isEmpty = 0;
    cache.cacheobjs[i].LRU = LRU_MAGIC_NUMBER++;

    writeAfter(i);/*writer V*/


}

/*find the empty cacheObj or which cacheObj should be evictioned*/
int cache_eviction() {
    int min = 0x7FFFFFFF;
    int minindex = 0;
    int i;
    for (i = 0; i < CACHE_OBJS_COUNT; i++) {
        readerPre(i);
        if (cache.cacheobjs[i].isEmpty == 1) {  /*choose if cache block empty */

            readerAfter(i);
            return i;
        }
        if (cache.cacheobjs[i].LRU < min) {    /*if not empty choose the min LRU*/
            min = cache.cacheobjs[i].LRU;
            minindex = i;
            readerAfter(i);
            continue;
        }
        readerAfter(i);
    }

    return minindex;
}


void readerPre(int i) {

    P(cache.cacheobjs[i].wmutex); // 在无写进程请求时进入
    P(cache.cacheobjs[i].cmutex); // 互斥访问count变量
    if (cache.cacheobjs[i].readCount == 0) //当第一个读进程读共享文件时
        P(cache.cacheobjs[i].rmutex); //阻止写进程写
    cache.cacheobjs[i].readCount++; //读者计数器加1
    V(cache.cacheobjs[i].cmutex); //释放互斥变量count
    V(cache.cacheobjs[i].wmutex); //恢复对共享文件的访问
}

void readerAfter(int i) {

    P(cache.cacheobjs[i].cmutex); //互斥访问count变量
    cache.cacheobjs[i].readCount--; //读者计数器减1
    if (cache.cacheobjs[i].readCount == 0) //当最后一个读进程读完共享文件
        V(cache.cacheobjs[i].rmutex); //允许写进程写
    V(cache.cacheobjs[i].cmutex); //释放互斥变量count
}

void writePre(int i) {

    P(cache.cacheobjs[i].wmutex); //在无写进程请求时进入
    P(cache.cacheobjs[i].rmutex); //互斥访问共享文件
}

void writeAfter(int i) {

    V(cache.cacheobjs[i].rmutex); // 释放共享文件
    V(cache.cacheobjs[i].wmutex); //恢复对共享支件的访问
}