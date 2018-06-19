#ifndef  __EXTERN_H__
#define __EXTERN_H__

#include"def.h"

extern char cmdline[MAXLINE+1];
extern command_t cmd[MAXPIPE];
extern char avline[MAXLINE+1]; 
extern int pipenum;
extern int lastpid;

#endif /* __EXTERN_H__ */
