#pragma once 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/epoll.h>


#define _DEFPATH_ "http.html"
#define _SIZE_ 1024
#define _CLIENTSIZE_ 5

void printferr(const char* file,const char* func,const int line);
void not_found(int sock);
void usage(const char * proc);
void not_found(int sock);
void echo_errno(int sock,int errno);
int startup(char *ip,int port);
int get_line(int sock,char*line,int line_size);
void clear_header(int sock);
void echo_html(int sock,char* path,ssize_t file_size);
void execute_cgi(int sock,const char* path,const char* method,char* query_string);
void accept_request(int sock);
void epoll_http(int listen_sock);
