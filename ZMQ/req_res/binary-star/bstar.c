#include "bstar.h"

/* 执行有限状态机(将事件绑定至状态) */
/* -异常返回-1 */
/* -正确返回0 */
static int s_execute_fsm(bstar_t *self){
    int rc = 0;
    //主机等待同伴连接
    //该状态下接收CLIENT_REQUEST事件
    if(self->state_ == STATE_PRIMARY){
        if(self->event_ == PEER_BACKUP){
            zclock_log("I : 已连接至备机(slave). 可以作为master运行");
            self->state_ = START_ACTIVE;
            if(self->master_fn_)
                (self->master_fn_)(self->loop_,NULL,self->master_arg_);
        }else if(self->event_ == PEER_ACTIVE){
            zclock_log("I : 已连接至备机(master)，可以作为slave运行");
            self->state_ = START_PASSIVE;
            if(self->slave_fn_)
                (self->slave_fn_)(self->loop_,NULL,self->slave_arg_);
        }else if(self->event_ == CLIENT_REQUEST){
            zclock_log("I : 收到客户端请求，可作为master运行");
            self->state_ = START_ACTIVE;
            if(self->master_fn_)
                (self->master_fn_)(self->loop_,NULL,self->master_arg_);
        }
    }else 
     /*
      * 备机等待同伴连接
      * 该状态下拒绝CLIENT_REQUSET事件
      */
     if(self->state_ == START_ACTIVE){
         if(self->event_ == PEER_ACTIVE){
             zclock_log("I : 已连接至主机(master)，可以作为slaver运行");
             self->state_ = START_PASSIVE;
             if(self->slave_fn_)
                 (self->slave_fn_)(self->loop_,NULL,self->slave_arg_);
         }else if(self->event_ == CLIENT_REQUEST){
             rc = -1;
         }
    }else
    /*
     * 服务器处于激活态
     * 该状态下接受CLIENT_REQUEST事件
     * 只有服务器死亡才会离开激活态
     */
     if(self->state_ == START_ACTIVE){
         if(self->event_ == PEER_ACTIVE){
              //若出现两台master,则抛出异常
              zclock_log("E : 严重错误: 双master。正在退出...");
              rc = -1;
          }
     }else 
     /*
      * 服务器处于被动态
      * 若同伴已死，CLIENT_REQUEST事件将触发故障恢复
      */ 
      if(self->state_ == START_PASSIVE){
          if(self->event_ == PEER_PRIMARY){
              /* 同伴正在重启 - 转为激活态，同伴将转为被动态 */
              zclock_log("I : 主机(primary) 正在重启，可作为master运行");
              self->state_ = START_ACTIVE;
          }else if(self->event_ == PEER_BACKUP){
              /* 同伴正在重启 - 转为激活态，同伴将转为被动态 */
              zclock_log("I : 备机(slave) 正在重启，可作为master运行");
              self->state_ = START_ACTIVE;
          }else if(self->event_ == PEER_PASSIVE){
              /*若出现两台slave，集群将无响应 */
              zclock_log("E : 严重错误: 双slave。正在退出...");
              rc = -1;
          }else if(self->event_ == CLIENT_REQUEST){
              /*
               * 若心跳超时，同伴将成为master
               * 此行为由客户端请求触发
               */
              assert(self->peer_expiry_ > 0);
              if(zclock_time() >= self->peer_expiry_){
                  /* 同伴已死，转为激活态 */  
                  zclock_log("I : 故障修复，可作为master运行");
                  self->state_ = START_ACTIVE;
              }else{
                  /* 同伴还在.拒绝请求 */
                  rc = -1;
              }
          }

          /* 触发状态更改事件处理函数 */
          if(self->state_ == START_ACTIVE && self->master_fn_)
              (self->master_fn_)(self->loop_,NULL,self->master_arg_);
      }
      return rc;
}

/* --------------------------------------------------------------- */
/* 反应堆事件处理程序 */

/* 发送状态信息 */
int s_send_state(zloop_t *loop,void *socket,void *arg){
    bstar_t *self = (bstar_t *)arg;    
    zstr_sendf(self->statepub_,"%d",self->state_);
    return 0;
}

/* 接收状态信息，启动有限状态机 */
int s_recv_state (zloop_t *loop,void *socket,void *arg){
    bstar_t *self = (bstar_t *)arg;
    char *state = zstr_recv(socket);
    if(state){
        self->event_ = atoi(state);
        self->peer_expiry_ = zclock_time() + 2 * BSTAR_HEARTBEAT;
        free(state);
    }

    return s_execute_fsm(self);
}


/* 收到应用程序请求，判断是否接收 */
int s_voter_ready(zloop_t *loop,void *socket,void *arg){
    bstar_t *self = (bstar_t *)arg;
    //如果能够处理请求，则调用函数
    self->event_ = CLIENT_REQUEST;
    if(s_execute_fsm(self) == 0){
        puts("CLIENT REQUEST");
        (self->voter_fn_)(self->loop_,socket,self->voter_arg_);
    }else{
        //销毁等待中的消息
        zmsg_t *msg = zmsg_recv(socket);
        zmsg_destroy(&msg);
    }
    return 0;
}


/* 构造函数 */
bstar_t *bstar_new(int primary,char *local,char *remote){
    bstar_t *self;
    self = (bstar_t *) zmalloc(sizeof(bstar_t));

    //初始化双子星
    self->ctx_ = zctx_new();
    self->loop_ = zloop_new();
    self->state_ = primary ? STATE_PRIMARY : STATR_BACKUP;

    //创建状态PUB套接字
    self->statepub_ = zsocket_new(self->ctx_,ZMQ_PUB);
    zsocket_bind(self->statepub_,local);

    //创建状态SUB套接字
    self->statesub_ = zsocket_new(self->ctx_,ZMQ_SUB);
    zsocket_connect(self->statesub_,remote);


    //设置基本的反应堆事件处理器
    zloop_timer(self->loop_,BSTAR_HEARTBEAT,0,s_send_state,self);
    zloop_reader(self->loop_,self->statesub_,s_recv_state,self);

    return self;
}

/* 析构函数 */
void bstar_destroy(bstar_t **self_p){
    assert(self_p);
    if(*self_p){
        bstar_t *self = *self_p;
        zloop_destroy(&self->loop_);
        zmq_ctx_destroy(&self->ctx_);
        free(self);
        *self_p = NULL;
    }
}

/* 返回底层zloop对象，用以添加额外的定时器，阅读器等 */
zloop_t *bstar_zloop(bstar_t *self){
    return self->loop_;
}

/* -创建套接字，连接至本地端点，注册成为阅读器 */
/* -只有当有限状态机允许时才会读取该套接字 */
/* -从该套接字获得的消息将作为一次"投票" */
/* -我们要求双子星模式中只有一个"投票套接字" */

int bstar_voter(bstar_t *self,char *endpoint,int type,zloop_fn handler,void *arg){
    //保存原始的回调函数和参数，稍后使用
    void *socket = zsocket_new(self->ctx_,type);
    zsocket_bind(socket,endpoint);
    assert(!self->voter_fn);
    self->voter_fn_ = handler;
    self->voter_arg_ = arg;

    return zloop_reader(self->loop_,socket,s_voter_ready,self);
}

/* 注册状态变化事件处理器 */
void bstar_new_master(bstar_t *self,zloop_fn handler,void *arg){
    assert(!self->master_fn);
    self->master_fn_ = handler;
    self->master_arg_ = arg;
}

void bstar_new_slave(bstar_t *self,zloop_fn handler,void *arg){
    assert(!self->slave_fn);
    self->slave_fn_ = handler;
    self->slave_arg_ = arg;
}

/* 启用或禁止跟踪信息 */
void bstar_set_verbose(bstar_t *self,bool verbose){
    zloop_set_verbose(self->loop_,verbose);
}

/* 开启反应堆，当回调函数返回-1，或进程收到SIGINT、SIGTERM信号时中止 */
int bstar_start(bstar_t *self){
    assert(self->voter_fn_);
    return zloop_start(self->loop_);
}
