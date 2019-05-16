#include <czmq.h>

/* 代理同伴模拟 */
/* 状态流原型 */

int main(int argc,char *argv[]){
    //第一个参数是代理的名称
    //其他参数是各个同伴的名称
    
    if(argc < 2){
       printf ("syntax:peering1 me {you}...\n");
       exit(EXIT_FAILURE);
    }

    char *self = argv[1];
    printf("I: 正准备代理程序 %s...\n",self);
    srandom((unsigned) time(NULL));

    //准备上下文和套接字
    zctx_t *ctx = zctx_new();
    void *statebe = zsocket_new(ctx,ZMQ_PUB);
    zsocket_bind(statebe,"ipc://s-state.ipc",self);

    //连接statefe套接字至所有同伴
    void *statefe = zsocket_new(ctx,ZMQ_SUB);
    for(int argn = 2;argn < argc;++argn){
        char *peer = argv[argn];
        printf("I: 正在连接至同伴代理 '%s' 的状态流后端",peer);
        zsocket_connect(statefe,"ipc://%s-state.ipc",peer);
    }

    //发送并接受状态消息
    //zmq_poll()函数使用的超时时间即心跳时间
    
    while(true){
        //初始化poll对象列表
        zmq_pollitem_t items [] = {
            {statefe,0,ZMQ_POLLIN,0}
        };

        //轮询套接字活动，超时时间为1秒
        int rc = zmq_poll(items,1,1000 * ZMQ_POLL_MSEC);
        if(rc == -1)
            break;  //中断

        //处理接收到的状态消息
        if(items[0].events & ZMQ_POLLIN){
            char *peer_name = zstr_recv(statefe);
            char *available = zstr_recv(statefe);
            printf("同伴代理 %s 有 %s 个work空闲\n",peer_name,available);
            free(peer_name);
            free(available);
        }else{
            //发送随机数表示空闲worker数
            zstr_sendm(statebe,self);
            zstr_sendf(statebe,"%d",randof(10));
        }
    }

    zctx_destroy(&ctx);
    return EXIT_SUCCESS;
}
