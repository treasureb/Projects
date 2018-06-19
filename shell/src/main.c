#include"../include/parse.h"
#include"../include/def.h"
#include"../include/init.h"

command_t cmd[MAXPIPE]; //命令行参数

char cmdline[MAXLINE+1];//读取命令行数据
char avline[MAXLINE+1]; //处理后的数据
int pipenum;            //当前使用的管道数
int lastpid;            //最后一个子进程ID

int main(){
    sig_init();
    shell_loop();
    return 0;
}
