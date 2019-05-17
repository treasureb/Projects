#ifndef  __BSTART__
#define __BSTART__

#include <czmq_library.h>
#include <czmq_prelude.h>
#include <czmq.h>

/* 服务器状态枚举 */
typedef enum {
    STATE_PRIMARY = 1,      //主机，等待同伴连接
    STATR_BACKUP  = 2,      //备机，等待同伴连接
    START_ACTIVE  = 3,      //激活态，处理应用程序请求
    START_PASSIVE = 4       //被动态，不接受请求
}state_t;

/* 对话节点事件 */
typedef enum {
    PEER_PRIMARY  = 1,      //主机
    PEER_BACKUP   = 2,      //备机
    PEER_ACTIVE   = 3,      //激活态
    PEER_PASSIVE  = 4,      //被动态
    CLIENT_REQUEST= 5       //客户端请求
}event_t;

struct bstar_t{
    zcxt_t*   ctx_;          //私有上下文
    zloop_t*  loop_;         //反应堆循环
    void*     statepub_;     //状态发布者
    void*     statesub_;     //状态订阅者
    state_t   state_;        //当前状态
    event_t   event_;        //当前事件
    int64_t   peer_expiry_;  //判定节点死亡的时限
    zloop_fn* voter_fn_;     //投票套接字处理器
    void*     voter_arg_;    //投票处理程序的参数
    zloop_fn* master_fn_;    //成为master时回调
    void*     master_arg_;   //参数
    zloop_fn* slave_fn_;     //成为slave时回调
    void*     slave_arg_;    //参数
};

#define BSTAR_HEARTBEAT     1000    //心跳时间

//创建双子星模式实例，使用本地(绑定)和远程(连接)端点来设置节点
bstar_t *bstar_new(int promary,char *local,char *remote);

//销毁函数
void bstar_destroy(bstar_t **self_p);

/* 返回底层的zloop反应堆，用以添加定时器，读取器，注册和取消等功能 */
zloop_t *bstar_zloop(bstar_t& self);

/* 注册投票读取器 */
int bstar_voter(bstar_t& self,char *endpoint,int type,
              zloop_fn handler,void *arg);

/* 注册状态机处理器 */
void bstar_new_master(bstar_t& self,zloop_fn handler,void *arg);
void bstar_new_slave(bstar_t& self,zloop_fn handler,void *arg);

/* 开启反应堆，当回调函数返回-1，或进程收到SIGINT,SIGTERM信号时中止 */
int bstar_start(bstar_t& self);
#endif  //endof __BSTART__
