#include"tetris.h"

/* 起始位置 */
struct data pos = {.x = 3,.y = 5};

void my_handler(int signal){
    tetris_timer(&pos);
}

void handler_int(int signal){
    printf("\033[?25h");
    recover_keyboard();
    exit(0);
}

int main(){
    /* 设置信号 */
    struct sigaction act;
    act.sa_flags = 0;
    act.sa_handler = my_handler;
    sigemptyset(&act.sa_mask);      /* 设置信号处理函数中的信号屏蔽字 */
    sigaction(SIGALRM,&act,NULL);

    signal(SIGINT,handler_int);
    signal(SIGQUIT,SIG_IGN);

    struct itimerval it;

    /* time out to run fun */
    it.it_value.tv_sec = 0;
    it.it_value.tv_usec = 1;

    /* interval time to run fun */
    it.it_interval.tv_sec = 1;
    it.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL,&it,NULL);

    init_shape_arg();
    while(1){
        tetris(&pos);
    }
    recover_keyboard();
}
