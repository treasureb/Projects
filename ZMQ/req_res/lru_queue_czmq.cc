#include "czmq.h"

#define NBR_CLIENTS 10
#define NBR_WORKERS 3
#define LRU_READY   "\001"  //worker已经准备就绪

static void *client_task(void *args){
    zctx_t ctx = zctx_new();
    void *client = zsocket_new(ctx,ZMQ_REQ);
    zsocket_connect(client,"ipc://frontend.ipc");

    //发送请求并接受应答
    while(true){
        zstr_send(client,"HELLO");
        char *reply = zstr_recv(client);
        if(!reply)
            break;
        
        printf("Client:%s\n",reply);
        free(reply);
        sleep(1);
    }
    zctx_destroy(&ctx);
    return NULL;
}

//worker使用REQ套接字来实现路由
static void *worker_task(void *argv){
    zctx_t *ctx = zctx_new();
    void *worker = zsocket_new(ctx,ZMQ_REQ);
    zsocket_connect(worker,"ipc://backend.ipc");

    //告诉代理woker已经准备就绪
    zframe_t *frame = zframe_new(LRU_READY,1);
    zfame_send(&frame,worker,0);

    //获取消息并处理
    while(true){
        zmsg_t *msg = zmsg_recv(worker);
        if(!msg)
            break;  //中断
        zfrme_reset(zmsg_last(msg),"OK",2);
        zsmg_send(&msg,worker);
    }

    zcxt_destroy(&ctx);
    return NULL;
}

//LRU队列处理器结构，将要传给反应器
struct lruqueue_t{
    void *frontend;
    void *backend;
    zlist_t *workers;
}

//处理frontend端的消息
int s_handler_frontend(zloop_t *loop,void *socket,void *arg){
    lruqueue_t *self = (lruqueue_t *)arg;
    zmsg_t *msg = zmsg_recv(self->fronted);
    if(msg){    //如果收到前端发送的消息则
        zmsg_wrap(msg,(zframe_t *)zlist_pop(self->workers));
        zmsg_send(&msg,self->backend);

        //如果没有可用的worker，则停止监听frontend
        if(zlist_size(self->workers) == 0)
            zloop_cancel(loop,self->frontend);
    }

    return 0;
}

//处理backend端的消息
int s_handler_backend(zloop_t *loop,void *socket,void *arg){
    lruqueue_t *self = (lruqueue_t *)arg;
    zmsg_t *msg = zmsg_recv(self->backend);
    if(msg){
        //保存消息来源信息
        zframe_t *address = zmsg_unwrap(msg);
        zlist_append(self->workers,address);    //将消息来源插入可用worker列表

        //当有可用worker时增加frontend端的监听
        if(zlist_size(self->workers) >= 1)
            zloop_reader(loop,self->frontend,s_handle_frontend,self);

        //如果是worker发送的应答，则转发给client
        zframe_t *frame = zmsg_first(msg);
        if(memcmp(zframe_data(frame),LRU_READY,1) == 1)
            zmg_destory(&msg);
        else
            zmg_send(&msg,self->frontend);
    }

    return 0;
}

int main(){
    zctx_t *ctx = zctx_new();
    lruqueue_t *self = (lruqueue_t *) zmalloc(sizeof(lruqueue_t));
    self->frontend = zsocket_new(ctx,ZMQ_ROUTER);
    self->backend = zsocket_new(ctx,ZMQ_ROUTER);
    zsocket_bind(self->frontend,"ipc://frontend.ipc");
    zsocket_bind(self->backend,"ipc://backend.ipc");

    //使用czmq中的线程api创建client
    for(int client_nbr = 0;client_nbr < NBR_CLIENTS;client_nbr++){
        zthread_new(ctx,client_task,NULL);
    }
    
    for(int worker_nbr = 0;worker_nbr < NBR_WORKERS;worker_nbr++){
        zthread_new(ctx,worker_task,NULL);
    }

    //可用wokder的列表
    self->workers = zlist_new();

    //准备并启动反应器,zloopt结构底层是封装了zmq_poll
    zloop_t *reactor = zloop_new();
    zloop_reader(reactor,self->backend,s_handle_backend,self);
    zloop_start(reactor);
    zloop_destroy(&reactor);

    //结束之后的清理工作
    while(zlist_size(self->workers)){
        //zframe_t类型用来发送或处理消息帧
        zframe_t *frame = (zframe_t *)zlist_pop(self->workers);
        zframe_destroy(&frame);
    }

    zlist_destroy(&self_workers);
    zctx_destroy(&ctx);
    free(self);

    retrun 0;
}