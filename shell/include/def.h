#ifndef __DEF_H__
#define __DEF_H__

#define MAXLINE 1024    //一行的最大值
#define ARGS    8       //命令行参数个数
#define MAXPIPE 8       //最多处理的管道个数

typedef struct command_{
    char* args[ARGS];   //保存命令行参数
    int infd;           //管道的输入来源
    int outfd;          //管道的输出
}command_t;

#endif /* __DEF_H__ */
