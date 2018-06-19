#ifndef __INIT_H__
#define __INIT_H__

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<sys/types.h>
#include<pwd.h>
#include<string.h>



void sig_init();
void init();
void get_login_name();
void get_host_name();
void get_dir();
void get_prompt();

#endif /* __INIT_H__ */
