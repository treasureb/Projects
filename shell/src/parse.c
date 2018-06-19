#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

#include"parse.h"
#include"extern.h"
#include"init.h"

void shell_loop(){
    while(1){
        get_prompt(); 
        init();
        if(cmd_read() == 0)
            break;
        cmd_parse();
        cmd_execute();
    }
    printf("\n");
}

/*
 * 读取命令行参数
 * 1.成功返回1
 * 2.失败返回0
 */
int cmd_read(){
    return fgets(cmdline,MAXLINE,stdin) == NULL ? 0:1;
}

/*
 *  解析命令行参数
 */
int cmd_parse(){
    char *p = cmdline;
    char *q = avline;
    
    for(int i = 0;i < MAXPIPE && *p != '\0';++i){
        for(int j = 0;j < ARGS && *p != '\0';++j){
            while(*p != '\0' && (*p == ' ' || *p == '\t' || *p == '\n')){
                p++;
            }
            cmd[i].args[j] = q;
            while(*p != '\0' && *p != ' ' && *p != '\t' && *p != '\n' && *p != '|'){
                *q++ = *p++;
            }
            *q++ = '\0';

            if(*p == '|'){
                pipenum++;
                p++;
                cmd[i].args[j] = NULL;
                break;
            }
            if(*p == '\n'){
                cmd[i].args[j+1] = NULL;
                break;
            }
        }     //end for int j = 0;i < ARGS
    }   //end for int i = 0;i < MAXPIPE
    return 0;
}

void cmd_print(){
    for(int i = 0;i <= pipenum;i++){
        printf("i = %d\n",i);
        for(int j = 0;cmd[i].args[j] != NULL;++j){
            printf("\t[%s]\n",cmd[i].args[j]);
        }
    } 
}

int cmd_execute(){
    int fds[2];
    for(int i = 0;i <= pipenum;++i){
        if(i< pipenum){
            pipe(fds);
            cmd[i].outfd = fds[1];
            cmd[i].infd = fds[0];
        }
        pid_t pid = fork();
        if(pid < 0){
            perror("fork");
        }else if(pid == 0){
            if(cmd[i].infd != 0){
                close(0);
                dup(cmd[i].infd);
            }
            if(cmd[i].outfd != 1){
                close(1);
                dup(cmd[i].infd);
            }
            execvp(cmd[i].args[0],cmd[i].args);
            exit(1);
        }else{
            if(cmd[i].infd != 0)
                close(cmd[i].infd);
            if(cmd[i].outfd != 1)
                close(cmd[i].outfd);
            lastpid = pid;
        }
    }   //end for int i = 0;i <= pipenum
    while(wait(NULL) != lastpid)
        ;

    return 0;
}



