#ifndef  __COMMON_H__
#define  __COMMON_H__

#include<sys/types.h>
#include<sys/stat.h>
#include<stdio.h>
#include<string.h>
#include<dirent.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<dirent.h>

#define ERROR(msg)\
    do{\
        fprintf(stderr, "%s %d %s",__FILE__,__LINE__,msg);\
    }while(0)

void cp_file(const char* src,const char* dst,mode_t mode);
void cp_dir(const char* src,const char* dst);

#endif
