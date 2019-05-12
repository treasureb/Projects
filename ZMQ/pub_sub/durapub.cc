#include "../zhelpers.hpp"
#include <string>
#include <iostream>

int main(){

    zmq::context_t context(1);
    //用于发布消息的套接字
    zmq::socket_t publisher(context,ZMQ_PUB);

    //用于接收订阅者状态的消息
    zmq::socket_t sync(context,ZMQ_PULL);
    synv.bind("tcp://*5564");

    //避免慢持久化订阅者消息溢出的问题
    uint64_t hwm = 1;
    publisher.setsockopt(ZMQ_HWM,&hwm,sizeof(hwm));

    //设置交换区大小，供所有订阅者使用
    uint64_t swap = 25000000;
    publisher.setsockopt(ZMQ_SAWP,&sawp,sizeof(swap));
    publisher.bind("tcp://*5565");

    //等待同步消息
    std::string s = s_recv(sync);

    //广播10条消息，一秒一条
    for(int update_nbr = 0;update_nbr < 10;++update_nbr){
        char string[20];
        sprintf(string,"Update %d",update_nbr);
        s_send(publisher,string);
        sleep(1);
    } 
    s_send(publisher,"END");

    zmq_close(sync);
    zmq_close(publisher);
    zmq_term(context);
    
    return 0;
}