/*
 * echo - read and echo text lines until client closes connection
 */
/* $begin echo */
#include "csapp.h"

void echo(int connfd)
{
    size_t n;
    char buf[MAXLINE];
    rio_t rio;

    Rio_readinitb(&rio, connfd);
    while((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) { //line:netp:echo:eof
      if(strcmp("ping\n", buf) == 0){
        printf("before pong\n");
        Rio_writen(connfd, "pong\n", n);
        printf("after pong\n");
      } else {
        printf("before bad\n");
        Rio_writen(connfd, "BAD REQUEST\n", 12);
        printf("after bad\n");
      }
    }
}
/* $end echo */

