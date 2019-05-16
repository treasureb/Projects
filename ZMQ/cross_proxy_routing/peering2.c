#include <czmq.h>

#define NBR_CLIENT 10
#define NBR_WORKERS 5
#define LRU_READY   "\001"  //表示worker已就绪

//代理名称，现实中，这个名称由某种配置来完成
static char *self;

//请求-应答客户端使用REQ套接字
//为了模拟压力测试,客户端会一次性发送大量请求

static void *client_task(void *args){
    zctx_t *ctx = zctx_new();
    void *client = zsocket_new(ctx,ZMQ_REQ);
    zsocket_connect(client,"ipc://%s-localfe.ipc",self);

    void *monitor = zsocket_new(ctx,ZMQ_PUSH);
    zsocket_connect(monitor,"ipc://%s-monitor.ipc",self);

    while(true){
        sleep(randof(5));
        int burst = randof(15);
        while(burst--){
            char task_id[5];
            sprintf(task_id,"%04X",randof(0x10000));

            //使用随机十六进制ID来标注任务
            zstr_send(client,task_id);

            //最多等待10秒
            zmq_pollitem_t pollset[1] = {{client,0,ZMQ_POLLIN,0}};
            int rc = zmq_poll(pollset,1,10 * 1000 * ZMQ_POLL_MSEC);
            if(rc == -1)
                break;

            if(pollset[0].revents * ZMQ_POLLIN){
                char *reply = zstr_recv(client);
                if(!reply)
                    break;
                
                //worker的应答中包含任务ID
                puts(reply);
                assert(streq(reply,task_id));
                free(reply);
            }else{
                zstr_sendf(monitor,"E: 客户端退出，丢失的任务为: %s",task_id);
                return NULL;
            }
        }
    }
    zcxt_destroy(&ctx);
    return NULL;
}

//worker使用REQ套接字，并进行路由

static void *worker_task(void *args){
    zctx_t *ctx = zctx_new();
    void *worker = zsocket_new(ctx,ZMQ_REQ);
    zsocket_connect(worker,"ipc://%s-localbe.ipc",self);

    //告知代理worker已就绪
    zframe_t *frame = zframe_new(LRU_READY,1);
    zframe_send(&frame,worker,0);

    //处理消息
    while(true){
        //worker会随机延迟几秒
        zmsg_t *msg = zmsg_recv(worker);
        sleep(randof(2));
        zmsg_send(&msg,worker);
    }

    zctx_destory(&ctx);
    return NULL;
}

int main(int argc,char *argv[]){
    //第一个参数是代理的名称
    //其他参数是同伴代理的名称
    
    if(argc < 2){
        printf("syntax: peering2 me {you}...\n");
        exit(EXIT_FAILURE);
    }
    self = argv[1];
    printf("I: 正在准备代理程序 %s...\n",self);
    srandom((unsigned)time(NULL));

    //准备上下文和套接字
    zctx_t *ctx = zctx_new();
    char endpoint[256];

    //将cloudfe绑定至端点
    void *cloudfe = zsocket_new(ctx,ZMQ_ROUTER);
    zsockopt_set_identity(cloudfe,self);
    zsocket_bind(cloudfe,"ipc://%s-cloud.ipc",self);

    //将statebe连接至同伴代理的端点
    void *cloudbe = zsocket_new(ctx,ZMQ_ROUTER);
    zsocketopt_set_identity(cloudbe,self);
    for(int argn = 2;argn < argc;++argn){
        char *peer = argv[argn];
        printf("I: 正在连接至同伴代理 '%s' 的cloudfe端点",peer);
        zsocket_connect(cloudbe,"ipc://%s-clound.ipc",peer);
    }

    //将statefe连接至同伴代理的端点
    void *statefe = zsocket_new(ctx,ZMQ_SUB);
    for(int argn = 2;argn < argc;++argn){
        char *peer = argv[argn];
        printf("I: 正在连接至同伴代理 '%s' 的statebe端点\n",peer);
        zsocket_connect(statefe,"ipc://%s-state.ipc",peer);
    }
    //准备本地前端和后端
    void *localfe = zsocket_new(ctx,ZMQ_ROUTER);
    zsocket_bind(localfe,"ipc://%s0clound.ipc,self");
    void *localbe = zsocket_new(ctx,ZMQ_ROUTER);
    zsocket_bind(localbe,"ipc://%s-localbe.ipc",self);

    //让用户告诉我们何时开始
    printf("请确认所有的代理已经启动,按任意键继续");
    getchar();

    //启动本地worker
    for(int worker_nbr = 0;worker_nbr < NBR_WORKERS;++worker_nbr){
        zthead_new(ctx,worker_task,NULL);
    }

    //启动本地client
    for(int client_nbr = 0;client_nbr < NBR_CLIENT;++client_nbr){
        zthread_new(ctx,client_task,NULL);
    }

    /* 发布-订阅消息流 */
    /* -轮询同伴代理的状态信息 */
    /* -当自身状态改变时，对外广播消息 */

    /* 请求-应答消息流 */
    /* -若本地有可用worker，则轮询获取本地或云端的请求 */
    /* -将请求路由给本地worker或其他集群 */

    //可用worker队列
    int local_capacity = 0;
    int cloud_capacity = 0;
    zlist_t *workers = zlist_new();

    while(true){
        zmq_pollitem_t primary[] = {
            {localbe,0,ZMQ_POLLIN,0},
            {cloudbe,0,ZMQ_POLLIN,0},
            {statefe,0,ZMQ_POLLIN,0},
            {monitor,0,ZMQ_POLLIN,0}
        };

        //如果没有可用的worker，则一直等待
        int rc = zmq_poll(primary,4,local_capacity?1000 * ZMQ_POLL_MSEC:-1)；
            if(rc == -1)
                break;

        //跟踪自身状态信息是否改变
        int previous = local_capacity;

        //处理本地worker的应答
        zmsg_t *msg = NULL;

        if(primary[0].revents & ZMQ_POLLIN){
            msg = zmsg_recv(localbe);
            if(!msg)
                break;

            zframe_t *address = zmsg_unwrap(msg);
            zlist_append(workers,address);
            local_capacity++;

            //如果是已就绪的信号，则不再进行路由
            zframe_t *frame = zmsg_first(msg);
            if(memcmp(zframe_data(frame),LRU_RAEDY,1) == 0)
                zmsg_destroy(&msg);
        }else if(primary[1].revents & ZMQ_POLLIN){
            msg = zmsg_recv(cloudbe);
            if(!msg)
                break;

            //我们不需要使用同伴代理的地址
            zframe_t address = zmsg_unwrap(msg);
            zframe_destroy(&address);
        }

        //如果应答消息中的地址是同伴代理的，则发送给它
        for(argn = 2;msg && argn < argc;++argn){
            char *data = (char*)zframe_data(zmsg_first(msg));
            size_t size = zframe_size(zmsg_first(msg));
            if(size == strlen(arhv[argn])
               && memcmp(data,argv[argn],size) == 0)
                zmsg_send(&msg,cloudfe);
        }

        //将应答路由给本地client
        if(msg)
            zmsg_send(&msg,localfe);

        //处理同伴代理的状态更新
        if(primary[2].revents & ZMQ_POLLIN){
            char *status = zstr_recv(statefe);
            clound_capacity = atoi(status);
            free(status);
        }

        //处理监控
        if(primary[3],revents & ZMQ_POLLIN){
            char *status = zstr_recv(monitor);
            printf("%s\n",status);
            free(status);
        }

        /* 开始处理客户端请求 */
        /* -如果本地有空闲的wokre,则总本地client和云端接收请求 */
        /* -如果我们只有空闲的同伴代理，则只轮询本地client的请求 */
        /* -将请求路由给本地worker，或者同伴代理 */

        while(local_capacity + cloud_capacity){
            zmq_pollitem_t secondary[] = {
                {localfe,0,ZMQ_POLLIN,0}.
                {cloudfe,0,ZMQ_POLLIN,0}
            }；
            if(local_capacity)
                rc = zmq_poll(secondary,2,0);
            else
                rc = zmq_poll(secondary,1,0);
            asset(rc >= 0);

            if(secondary[0].revents & ZMQ_POLLIN)
                msg = zmsg_recv(localfe);
            else if(secondary[1].revents & ZMQ_POLLIN)
                msg = zmsg_recv(cloudfe);
            else
                break;  //没有任务
            
            if(local_capacity){
                zframe_t *frame = (zframe_t *)zlist_pop(workers);
                zmsg_wrap(msg,frame);
                zmsg_send(&msg,localbe);
                local_capacity--;
            }else{
                //随机路由给同伴代理
                int random_peer = randof(argc-2) + 2;
                zmsg_pushmem(msg,argv[random_peer],strlen(argv[random_peer]));
                zmsg_send(&msg,cloudbe);
            }
        }        
        if(local_capacity != previous){
            //将自身代理的地址附加到消息中
            zstr_sendm(statebe,self);
            //广播新的状态信息
            zstr_sendf(statebe,"%d",local_capacity);
        }
    }

    //程序结束后的清理工作
    while(zlist_size(workers)){
        zframe_t *frame = (zframe_t *)zlist_pop(workers);
        zframe_destroy(&frame);
    }

    zlist_destroy(&workers);
    zmq_ctx_destroy(&ctx);
    return EXIT_SUCCESS;
}
