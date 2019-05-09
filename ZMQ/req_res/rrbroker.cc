#include "zhelpers.hpp"

int main(){
    //准备上下文和套接字
    void *context = zmq_init(1);
    void *frontend = zmq_socket(context,ZMQ_ROUTER);
    void *backend = zmq_socket(context,ZMQ_DEALER);
    zmq_bind(frontend,"tcp://*:5559");
    zmq_bind(backend,"tcp://*:5560");

    //初始化轮询集合
    zmq_pollitem_t items[] = {
        {frontend,0,ZMQ_POLLIN,0},
        {backend,0,ZMQ_POLLIN,0}
    };

    //在套接字间转发消息
    while(true){
        zmq_msg_t message;
        int64_t more;

        zmq_poll(items,2,-1);
        if(items[0].revents & ZMQ_POLLIN){
            while(true){
                //处理所有消息帧
                zmq_msg_init(&message);
                zmq_recv(frontemd,&message,0);
                size_t more_size = sizeof(more);
                zmq_getsockopt(frontend,ZMQ_RCVMORE,&message);
                zmq_send(backend,&message,more?ZMQ_SNDMORE,0);
                zme_msg_close(&message);
                if(!more)
                break;
            }
        }
        if(items[1].revents & ZMQ_POLLIN){
            while(true){
                //处理所有消息帧
                zmq_msg_init(&message);
                zmq_recv(backend,&message,0);
                size_t more_size = sizeof(more);
                zmq_getsockopt(backend,ZMQ_RCVMORE,&more,&more_size);
                zmq_send(frontend,&message,more?ZMQ_SNDMORE:0);
                zmq_msg_close(&message);
                if(!more)
                    break;
            }
        }
    }

    zmq_close(frontend);
    zmq_close(backend);
    zmq_term(context);
    return 0;
}
