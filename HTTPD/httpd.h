#ifndef _HTTPD_H_
#define _HTTPD_H_

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<fcntl.h>
#include<errno.h>
#include<string.h>
#include<unistd.h>
#include<ctype.h>
#include<sys/stat.h>
#include<sys/sendfile.h>
#include<sys/wait.h>

#define SICCESS 0
#define NOTICE  1
#define WARNING 2
#define ERROR   3
#define FATAL   4

#define SIZE 1024

int startup(const char* ip,int port);
void print_log(const char* error,int level);
void *handler_request(void *arg);
static int Getline(long sock,char line[],int len);
#endif
