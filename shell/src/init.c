#include"init.h"
#include"extern.h"

void handler(int signal){
    printf("\n[sxshell]: ");
    fflush(stdout);
}
void sig_init(){
    signal(SIGINT,handler);
    signal(SIGQUIT,SIG_IGN);
}

void get_host_name(){
    char hostname[1024] = {0};
    gethostname(hostname,sizeof(hostname)-1);
    printf("%s",hostname);
}

void get_login_name(){
    struct passwd* pass;
    pass = getpwuid(getuid());
    printf("[%s@",pass->pw_name);
} 

void get_dir(){
    char pwd[128];
    getcwd(pwd,sizeof(pwd)-1);
    int len = strlen(pwd);
    char* p = pwd + len -1;
    while(*p != '/' && len--){
        p--;
    }
    p++;
    printf("%s]#",pwd);
}

void get_prompt(){
    get_login_name();
    get_host_name();
    get_dir();
}

void init(){
    memset(cmdline,0x0,sizeof(cmdline));
    memset(avline,0x0,sizeof(avline));
    for(int i = 0;i < MAXPIPE;++i){
        for(int j = 0;j < ARGS;j++){
            cmd[i].args[j] = NULL;
        }
        cmd[i].infd = 0;
        cmd[i].outfd = 1;
    }
    pipenum = 0;
}

