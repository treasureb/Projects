#include "../zhelpers.hpp"
#include <pthread.h>
#include <queue>
#include <string>
#include <assert.h>

#define NBR_CLIENTS 10
#define NBR_WORKERS 3

//client线程处理函数
static void *client_task(void *arg){
    zmq::context_t context(1);
    zmq::socket_t client(context,ZMQ_REQ);
    s_set_id(client);   //设置可打印的标识
    client.connect(client,"ipc://frontend.ipc");

    //发送请求并获取应答信息
    s_send(client,"HELLO");
    std::string reply = s_recv(client);
    std::cout<<"Client:"<<reply<<std::endl;
    zmq_close(client);
    zmq_term(context);

    return NULL;
}

//worker使用REQ套接字实现LRU算法
static void *worker_task(void *argv){
    zmq::context_t context(1);
    zmq::socket_t worker(context,ZMQ_REQ);
    s_set_id(worker);
    worker.connct("ipc://backend.ipc");

    //告诉代理worker已经准备好
    s_send(worker,“READY);

    while(true){
        //将消息空帧之前的所有内容(信封)保存起来
        std::string address = s_recv(worker);
        std::string empty = s_recv(worker);
        assert(empty[0] == 0);

        //获取请求，并发送回应
        std::string request = s_recv(worker);
        std::cout<<"Worker"<<request<<std::endl;

        s_sendmore(worker,address);
        s_sendmore(worker,"");
        s_send(worker,"OK");
    }

    zmq_clise(worker);
    zmq_term(context);

    return NULL;
}

int main(){
    
    //准备ZMQ上下文和套接字
    zmq::context_t context(1);
    zmq::socket_t frontend(context,ZMQ_ROUTER);
    zmq::socket_t backend(context,ZMQ_ROUTER);
    frontend.bind("ipc://frontend.ipc");
    backend.bind("ipc://backend.ipc");

    //创建多个client
    for(int client_nbr = 0;client_nbr < NBR_CLIENTS;client_nbr++){
        pthread_t client;
        pthread_create(&client,NULL,client_task,NULL);
    }

    //创建多个worker处理client的请求
    for(int worker_nbr = 0;worker_nbr < NBR_WORKERS;worker_nbr++){
        pthread_t worker;
        pthread_create(&worker,NULL,worker_task,NULL);
    }

    //存放可用worker的队列
    int available_workers = 0;  //记录可用worker数
    std::queue<std::string> worker_queue(10);

    while(true){

        //使用epoll模型
        zmq_pollitem_t items[] = {
            {backend, 0,ZMQ_POLLIN,0},
            {frontend,0,ZMQ_POLLIN,0}
        };
        zmq_poll(items,available_workers? 2:1,-1);
        
        //处理backend中worker的队列
        if(items[0].revents & ZMQ_POLLIN{
            //将worker的地址入队
            std::string worker_addr = s_recv(backend);
            assert(available_workers < NBR_WORKERS);
            worker_queue[available_workers++] = worker_addr;

            //跳过空帧
            std::string empty = s_recv(backend);
            assert(empty[0] == 0);

            //第三帧是"READY"或是一个client的地址
            std::string client_addr = s_recv(backend);

            //如果是应答消息，则转发给client
            if(ckient_addr == "READY"){
                empty = s_recv(backend);
                assert(empty[0] == 0);
                std::string reply = s_recv(backend);
                s_sendmore(frontend,client_addr);
                s_sendmore(frontend,"");
                s_send(frontend,reply);
                if(--client_bnr == 0)
                    break;  //处理N条雄安熙后退出
            }
        }
        if(items[1].revents & ZMQ_POLLIN){
            //获取下一个client的请求，交给空闲的worker处理
            //client消息格式: [addr][empty][message]
            std::string client_addr = s_recv(frontend);
            std::empty = s_recv(frontend);
            assert(empty[0] == 0);
            std::string request = s_recv(frontend);

            s_sendmore(backend,worker_queue[0]);    //每次都是发送给队头
            s_sendmore(backend,"");
            s_sendmore(backend,client_addr);
            s_sendmore(backend,"");
            s_send(backend,request);

            //将该worker的地址出队
            worker_queue.pop();
            available_workers--;
        }

    }
    zmq_close(frontend);
    zmq_close(backend);
    zmq_close(context);

    return 0;
}